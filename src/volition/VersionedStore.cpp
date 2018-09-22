// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/VersionedStore.h>

//#define DEBUG_LOG printf ( "%04x:  ", ( int )(( size_t )this ) & 0xffff ); printf
#define DEBUG_LOG(...)

namespace Volition {

//================================================================//
// VersionedStore
//================================================================//

//----------------------------------------------------------------//
/** \brief  Create a branch if none exists and add self as client.
*/
void VersionedStore::affirmBranch () {

    if ( !this->mBranch ) {
        assert ( this->mVersion == 0 );
        this->setBranch ( make_shared < VersionedStoreBranch >(), this->mVersion );
    }
}

//----------------------------------------------------------------//
/** \brief  Abandons branch and sets version to 0.
*/
void VersionedStore::clear () {

    this->setBranch ( NULL, 0 );
}

//----------------------------------------------------------------//
/** \brief  Return a void pointer to the value for key.

    Searches back through the store for the most recent value assigned
    to the key. If no value for the key is found in the history, NULL
    is returned.
 
    \param  key         Find the value using this key.
    \param  version     Search backward for the value starting at this version.
    \param  typeID      Has of the value's typeid. Used as a sanity check.
    \return             A raw pointer to the value's memory or NULL.
*/
const void* VersionedStore::getRaw ( string key, size_t version, size_t typeID ) const {

    return this->mBranch ? this->mBranch->getRaw ( version < this->mVersion ? version : this->mVersion, key, typeID ) : NULL;
}

//----------------------------------------------------------------//
/** \brief  Return the current version.

    \return             The current version.
*/
size_t VersionedStore::getVersion () const {

    return this->mVersion;
}

//----------------------------------------------------------------//
/** \brief  Return the version depended on in the branch by this cursor.
            This version must not be altered by any other cursor.

    All values are held in a branch. The "dependent version" is the
    version below which all layers must be unique to the cursor. If any other
    cursor needs to alter any layer below the depdent layer, a new branch
    must be created to hold the changes. The new branch will contain a
    full copy of the layer being changed.
 
    For cursors, since they reference the "current version" where
    values will be written, the "dependent version" is the mVersion + 1.
 
    \return             The dependent version.
*/
size_t VersionedStore::getVersionDependency () const {

    return this->mVersion + 1;
}

//----------------------------------------------------------------//
/** \brief  Pop the top version and revert to the version before it.
*/
void VersionedStore::popVersion () {

    LOG_SCOPE_F ( INFO, "VersionedStore::  ()" );

    if ( this->mBranch ) {
    
        size_t version = this->mVersion;
        shared_ptr < VersionedStoreBranch > branch = this->mBranch;
        branch->eraseClient ( *this );
        
        this->mBranch = NULL;
        this->mVersion = 0;
        
        if ( version == branch->mBaseVersion ) {
            branch = branch->mParent;
        }
        
        if ( branch ) {
            branch->affirmClient ( *this );
            this->mBranch = branch;
            this->mVersion = version - 1;
            this->mBranch->optimize ();
        }
    }
}

//----------------------------------------------------------------//
/** \brief  Make sure a branch exists to hodl the value. If a branch already exists,
            create a new branch if setting a value for a version with dependencies.
 
    This function find the top version in the store below which all other versions
    should be considered immutable. The immutable top is simply the highest version
    returned by getVersionDependency() for each cursor and branch other than the
    current cursor.
 
    If the current cursor's version is below the immutable top, then a new branch
    must be created. The new branch will become a child of the original branch
    and the cursor will be moved to this new branch.
*/
void VersionedStore::prepareForSetValue () {

    LOG_SCOPE_F ( INFO, "VersionedStore::prepareForSetValue ()" );

    this->affirmBranch ();
    
    size_t dependencies = this->mBranch->countDependencies ();
    if ( dependencies > 1 ) {
    
        size_t immutableTop = this->mBranch->findImmutableTop ( this );
        LOG_F ( INFO, "immutableTop: %d", ( int )immutableTop );
        
        if ( this->mVersion < immutableTop ) {
        
            LOG_F ( INFO, "SPLIT!" );
        
            shared_ptr < VersionedStoreBranch > branch = this->mBranch;
            this->mBranch->eraseClient ( *this );
            this->mBranch = make_shared < VersionedStoreBranch >( this->mBranch, this->mVersion );
            this->mBranch->affirmClient ( *this );
            branch->optimize (); // not sure this will ever do anything
        }
    }
}

//----------------------------------------------------------------//
/** \brief  Increment the version.
 
    This function just increments the version counter. Since layers
    are created sparsely (and only on write), nothing gets created by
    this function unless the current version is less than the top
    version held in the branch. In this case, a child branch is created
    to represent the "empty" version and the cursor is moved to that
    branch.
*/
void VersionedStore::pushVersion () {

    LOG_SCOPE_F ( INFO, "VersionedStore::pushVersion ()" );

    if ( !this->mBranch ) {
        this->mVersion = 0;
    }

    this->affirmBranch ();
    assert ( this->mBranch );

    this->mVersion++;
    LOG_F ( INFO, "version: %d", ( int )this->mVersion );
    
    if ( this->mVersion < this->mBranch->getTopVersion ()) {
    
        LOG_F ( INFO, "SPLIT" );
    
        shared_ptr < VersionedStoreBranch > branch = this->mBranch;
        this->mBranch->eraseClient ( *this );
        this->mBranch = make_shared < VersionedStoreBranch >( this->mBranch, this->mVersion - 1 );
        this->mBranch->affirmClient ( *this );
        branch->optimize (); // not sure this will ever do anything
    }
}

//----------------------------------------------------------------//
/** \brief  Remove the cursor from the existing branch (if any) and add
            it to the new branch.

    Branches internally maintain a set of their clients. This method updates
    the branch client sets correctly. Any version may be specified that
    is greater than the base version of the new branch. This will add a
    dependency on all lesser versions held in the branch.
 
    When the cursor is removed from its original, the original branch
    will be deleted or optimized. The original branch is only deleted
    if the cursor was the last reference to it.
 
    \param  branch      The new branch for the cursor.
    \param  version     The version referenced by the cursor.
*/
void VersionedStore::setBranch ( shared_ptr < VersionedStoreBranch > branch, size_t version ) {

    weak_ptr < VersionedStoreBranch > prevBranchWeak;

    if ( this->mBranch != branch ) {
        
        LOG_SCOPE_F ( INFO, "VersionedStore::setBranch () - changing branch" );
        
        if ( this->mBranch ) {
            prevBranchWeak = this->mBranch;
            this->mBranch->eraseClient ( *this );
        }
        
        this->mBranch = branch;
        branch = NULL;
    }
    
     if ( this->mBranch ) {
        assert ( version >= this->mBranch->mBaseVersion );
        this->mVersion = version;
        this->mBranch->affirmClient ( *this );
        this->mBranch->optimize ();
    }
    else {
        this->mVersion = 0;
    }

    if ( !prevBranchWeak.expired ()) {
        branch = prevBranchWeak.lock ();
        branch->optimize ();
    }
}

//----------------------------------------------------------------//
/** \brief  Revert to a previous version.
 
    This method may trigger branch optimizations and deletions. Once
    reverted, there is no way to seek forward again. Use takeSnapshot()
    before calling revert() if it's important to be able to return
    to the current version.
 
    \param  version     The version to revert to.
*/
void VersionedStore::revert ( size_t version ) {

    LOG_SCOPE_F ( INFO, "VersionedStore::rewind ( %d )", ( int )version );

    assert ( version <= this->mVersion );
    
    if (( this->mBranch ) && ( version < this->mVersion )) {
    
        shared_ptr < VersionedStoreBranch > branch = this->mBranch;
        branch->eraseClient ( *this );
        this->mBranch = NULL;
        
        size_t top = this->mVersion;
        for ( ; branch && ( version < branch->mBaseVersion ); branch = branch->mParent ) {
            top = branch->mBaseVersion;
        }
        
        assert ( branch );
        assert ( branch->mBaseVersion <= version );
        assert ( version < top );
        
        branch->affirmClient ( *this );
        this->mBranch = branch;
        this->mVersion = version;
        this->mBranch->optimize ();
    }
}

//----------------------------------------------------------------//
/** \brief  Set the debug name. Implemented in _DEBUG builds only. Otehrwise a no-op.
 
    \param  debugName   The debug name.
*/
void VersionedStore::setDebugName ( string debugName ) {

    #ifdef _DEBUG
        this->mDebugName = debugName;
    #endif
}

//----------------------------------------------------------------//
/** \brief  Set the value of a key using a raw pointer to its data.
 
    The value type should be checked and the branch's corresponding ValueStack
    should be created before calling this method. It's up to the ValueStack
    implementation to interpret the raw pointer correctly.
 
    \param  key         The key.
    \param  value       The value.
*/
void VersionedStore::setRaw ( string key, const void* value ) {

    LOG_SCOPE_F ( INFO, "VersionedStore::setRaw ( %s, %p )", key.c_str (), value );

    assert ( this->mBranch );
    this->mBranch->setRaw ( this->mVersion, key, value );
}

//----------------------------------------------------------------//
/** \brief  Take a snapshot of the current version.

    This is a relatively low-cost operation. Taking a snapshot will
    add a dependency on the shared branch but won'y do anything else
    until the branch is altered.
 
    \param  other   The version to snapshot.
*/
void VersionedStore::takeSnapshot ( VersionedStore& other ) {

    this->setBranch ( other.mBranch, other.mVersion );
}

//----------------------------------------------------------------//
VersionedStore::VersionedStore () :
    mVersion ( 0 ) {
}

//----------------------------------------------------------------//
VersionedStore::VersionedStore ( VersionedStore& other ) {

    this->takeSnapshot ( other );
}

//----------------------------------------------------------------//
VersionedStore::~VersionedStore () {

    weak_ptr < VersionedStoreBranch > branchWeak = this->mBranch;
    this->setBranch ( NULL, 0 );
    if ( !branchWeak.expired ()) {
        shared_ptr < VersionedStoreBranch > branch = branchWeak.lock ();
        branch->optimize ();
    }
}

} // namespace Volition
