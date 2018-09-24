// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/AbstractVersionedBranchClient.h>
#include <volition/VersionedBranch.h>

namespace Volition {

//================================================================//
// AbstractVersionedBranchClient
//================================================================//

//----------------------------------------------------------------//
AbstractVersionedBranchClient::AbstractVersionedBranchClient () :
    mVersion ( 0 ) {
}

//----------------------------------------------------------------//
AbstractVersionedBranchClient::~AbstractVersionedBranchClient () {
    this->setBranch ( NULL );
}

//----------------------------------------------------------------//
bool AbstractVersionedBranchClient::canJoin () const {
    return this->AbstractVersionedStoreClient_canJoin ();
}

//----------------------------------------------------------------//
size_t AbstractVersionedBranchClient::getJoinScore () const {
    return this->AbstractVersionedStoreClient_getJoinScore ();
}

//----------------------------------------------------------------//
size_t AbstractVersionedBranchClient::getVersionDependency () const {
    return this->AbstractVersionedStoreClient_getVersionDependency ();
}

//----------------------------------------------------------------//
void AbstractVersionedBranchClient::joinBranch ( VersionedBranch& branch ) {
    this->AbstractVersionedStoreClient_joinBranch ( branch );
}

//----------------------------------------------------------------//
bool AbstractVersionedBranchClient::preventJoin () const {
    return this->AbstractVersionedStoreClient_preventJoin ();
}

//----------------------------------------------------------------//
/** \brief Remove the client from the existing branch (if any) and add
    it to the new branch without changing the client's version.
 
    \param  branch      The new branch for the snapshot.
*/
void AbstractVersionedBranchClient::setBranch ( shared_ptr < VersionedBranch > branch ) {
    this->setBranch ( branch, this->mVersion );
}

//----------------------------------------------------------------//
/** \brief Remove the client from the existing branch (if any) and add
    it to the new branch. Update the client's version.

    Branches internally maintain a set of their clients. This method updates
    the branch client sets correctly. Any version may be specified that
    is greater than the base version of the new branch. This will add a
    dependency on all lesser versions held in the branch.
 
    When the snapshot is removed from its original, the original branch
    will be deleted or optimized. The original branch is only deleted
    if the snapshot was the last reference to it.
 
    \param  branch      The new branch for the snapshot.
    \param  version     The version referenced by the snapshot.
*/
void AbstractVersionedBranchClient::setBranch ( shared_ptr < VersionedBranch > branch, size_t version ) {

    weak_ptr < VersionedBranch > prevBranchWeak;

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
        assert ( version >= this->mBranch->mVersion );
        this->mVersion = version;
        this->mBranch->insertClient ( *this );
    }
    else {
        this->mVersion = 0;
    }

    if ( !prevBranchWeak.expired ()) {
        branch = prevBranchWeak.lock ();
        branch->optimize ();
    }
}

} // namespace Volition
