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
AbstractVersionedBranchClient::AbstractVersionedBranchClient () {
}

//----------------------------------------------------------------//
AbstractVersionedBranchClient::~AbstractVersionedBranchClient () {
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

} // namespace Volition
