// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/AbstractVersionedStoreClient.h>
#include <volition/VersionedStoreBranch.h>

namespace Volition {

//================================================================//
// AbstractVersionedStoreClient
//================================================================//

//----------------------------------------------------------------//
AbstractVersionedStoreClient::AbstractVersionedStoreClient () {
}

//----------------------------------------------------------------//
AbstractVersionedStoreClient::~AbstractVersionedStoreClient () {
}

//----------------------------------------------------------------//
bool AbstractVersionedStoreClient::canJoin () const {
    return this->AbstractVersionedStoreClient_canJoin ();
}

//----------------------------------------------------------------//
size_t AbstractVersionedStoreClient::getJoinScore () const {
    return this->AbstractVersionedStoreClient_getJoinScore ();
}

//----------------------------------------------------------------//
size_t AbstractVersionedStoreClient::getVersionDependency () const {
    return this->AbstractVersionedStoreClient_getVersionDependency ();
}

//----------------------------------------------------------------//
void AbstractVersionedStoreClient::joinBranch ( VersionedStoreBranch& branch ) {
    this->AbstractVersionedStoreClient_joinBranch ( branch );
}

//----------------------------------------------------------------//
bool AbstractVersionedStoreClient::preventJoin () const {
    return this->AbstractVersionedStoreClient_preventJoin ();
}

} // namespace Volition
