// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/VersionedStore.h>

namespace Volition {

//================================================================//
// VersionedStore
//================================================================//

//----------------------------------------------------------------//
/** \brief Pop the top version and revert to the version before it.
*/
void VersionedStore::popVersion () {

    LOG_SCOPE_F ( INFO, "VersionedStore::  ()" );

    if ( this->mBranch ) {
    
        size_t version = this->mVersion;
        shared_ptr < VersionedBranch > branch = this->mBranch;
        branch->eraseClient ( *this );
        
        this->mBranch = NULL;
        this->mVersion = 0;
        
        if ( version == branch->mBaseVersion ) {
            branch = branch->mBranch;
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
/** \brief Make sure a branch exists to hodl the value. If a branch already exists,
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
        
            this->mBranch->eraseClient ( *this );
            this->mBranch = make_shared < VersionedBranch >( this->mBranch, this->mVersion );
            this->mBranch->affirmClient ( *this );
        }
    }
}

//----------------------------------------------------------------//
/** \brief Increment the version.
 
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
    
        this->mBranch->eraseClient ( *this );
        this->mBranch = make_shared < VersionedBranch >( this->mBranch, this->mVersion - 1 );
        this->mBranch->affirmClient ( *this );
    }
}

//----------------------------------------------------------------//
/** \brief Revert to a previous version.
 
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
    
        shared_ptr < VersionedBranch > branch = this->mBranch;
        branch->eraseClient ( *this );
        this->mBranch = NULL;
        
        size_t top = this->mVersion;
        for ( ; branch && ( version < branch->mBaseVersion ); branch = branch->mBranch ) {
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
/** \brief Set the value of a key using a raw pointer to its data.
 
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
VersionedStore::VersionedStore () {
}

//----------------------------------------------------------------//
VersionedStore::VersionedStore ( VersionedStoreSnapshot& other ) :
    VersionedStoreSnapshot ( other ) {
}

//----------------------------------------------------------------//
VersionedStore::~VersionedStore () {
}

} // namespace Volition
