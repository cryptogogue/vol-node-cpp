// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/AbstractVersionedStoreEpochClient.h>
#include <volition/VersionedStoreEpoch.h>

namespace Volition {

//================================================================//
// AbstractVersionedStoreEpochClient
//================================================================//

//----------------------------------------------------------------//
AbstractVersionedStoreEpochClient::AbstractVersionedStoreEpochClient () {
}

//----------------------------------------------------------------//
AbstractVersionedStoreEpochClient::~AbstractVersionedStoreEpochClient () {
}

//----------------------------------------------------------------//
size_t AbstractVersionedStoreEpochClient::getVersion () const {

    return this->AbstractVersionedStoreEpochClient_getVersion ();
}


} // namespace Volition
