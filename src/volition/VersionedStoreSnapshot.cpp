// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/VersionedStoreSnapshot.h>

namespace Volition {

//================================================================//
// VersionedStoreSnapshot
//================================================================//

//----------------------------------------------------------------//
/** \brief Create a branch if none exists and add self as client.
*/
void VersionedStoreSnapshot::affirmBranch () {

    if ( !this->mBranch ) {
        assert ( this->mVersion == 0 );
        this->setBranch ( make_shared < VersionedStoreBranch >(), this->mVersion );
    }
}

//----------------------------------------------------------------//
/** \brief Abandons branch and sets version to 0.
*/
void VersionedStoreSnapshot::clear () {

    this->setBranch ( NULL, 0 );
}

//----------------------------------------------------------------//
/** \brief Return a void pointer to the value for key.

    Searches back through the store for the most recent value assigned
    to the key. If no value for the key is found in the history, NULL
    is returned.
 
    \param  key         Find the value using this key.
    \param  version     Search backward for the value starting at this version.
    \param  typeID      Has of the value's typeid. Used as a sanity check.
    \return             A raw pointer to the value's memory or NULL.
*/
const void* VersionedStoreSnapshot::getRaw ( string key, size_t version, size_t typeID ) const {

    return this->mBranch ? this->mBranch->getRaw ( version < this->mVersion ? version : this->mVersion, key, typeID ) : NULL;
}

//----------------------------------------------------------------//
/** \brief Return the current version.

    \return             The current version.
*/
size_t VersionedStoreSnapshot::getVersion () const {

    return this->mVersion;
}

//----------------------------------------------------------------//
/** \brief Return the version depended on in the branch by this cursor.
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
size_t VersionedStoreSnapshot::getVersionDependency () const {

    return this->mVersion + 1;
}

//----------------------------------------------------------------//
/** \brief Remove the cursor from the existing branch (if any) and add
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
void VersionedStoreSnapshot::setBranch ( shared_ptr < VersionedStoreBranch > branch, size_t version ) {

    weak_ptr < VersionedStoreBranch > prevBranchWeak;

    if ( this->mBranch != branch ) {
        
        LOG_SCOPE_F ( INFO, "VersionedStoreSnapshot::setBranch () - changing branch" );
        
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
/** \brief Set the debug name. Implemented in _DEBUG builds only. Otehrwise a no-op.
 
    \param  debugName   The debug name.
*/
void VersionedStoreSnapshot::setDebugName ( string debugName ) {

    #ifdef _DEBUG
        this->mDebugName = debugName;
    #endif
}

//----------------------------------------------------------------//
/** \brief Take a snapshot of the current version.

    This is a relatively low-cost operation. Taking a snapshot will
    add a dependency on the shared branch but won'y do anything else
    until the branch is altered.
 
    \param  other   The version to snapshot.
*/
void VersionedStoreSnapshot::takeSnapshot ( VersionedStoreSnapshot& other ) {

    this->setBranch ( other.mBranch, other.mVersion );
}

//----------------------------------------------------------------//
VersionedStoreSnapshot::VersionedStoreSnapshot () :
    mVersion ( 0 ) {
}

//----------------------------------------------------------------//
VersionedStoreSnapshot::VersionedStoreSnapshot ( VersionedStoreSnapshot& other ) {

    this->takeSnapshot ( other );
}

//----------------------------------------------------------------//
VersionedStoreSnapshot::~VersionedStoreSnapshot () {

    weak_ptr < VersionedStoreBranch > branchWeak = this->mBranch;
    this->setBranch ( NULL, 0 );
    if ( !branchWeak.expired ()) {
        shared_ptr < VersionedStoreBranch > branch = branchWeak.lock ();
        branch->optimize ();
    }
}

} // namespace Volition
