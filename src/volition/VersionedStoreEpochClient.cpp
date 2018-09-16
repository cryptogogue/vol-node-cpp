// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/VersionedStore.h>
#include <volition/VersionedStoreEpoch.h>
#include <volition/VersionedStoreEpochClient.h>

namespace Volition {

//================================================================//
// VersionedStoreEpochClient
//================================================================//

//----------------------------------------------------------------//
size_t VersionedStoreEpochClient::getVersion () const {

    return this->mVersion;
}

//----------------------------------------------------------------//
void VersionedStoreEpochClient::setEpoch ( shared_ptr < VersionedStoreEpoch > epoch ) {

    this->setEpoch ( epoch, epoch ? epoch->mVersion : 0 );
}

//----------------------------------------------------------------//
void VersionedStoreEpochClient::setEpoch ( shared_ptr < VersionedStoreEpoch > epoch, size_t version ) {

    if ( epoch != this->mEpoch ) {
        
        if ( this->mEpoch ) {
            this->mEpoch->mClients.erase ( this );
            this->mEpoch->optimize ();
        }
        
        this->mEpoch = epoch;
        
        if ( epoch ) {
            epoch->mClients.insert ( this );
        }
    }
    // TODO: add sanity check
    this->mVersion = version;
}

//----------------------------------------------------------------//
VersionedStoreEpochClient::VersionedStoreEpochClient () :
    mVersion ( 0 ) {
}

//----------------------------------------------------------------//
VersionedStoreEpochClient::~VersionedStoreEpochClient () {

    this->setEpoch ( NULL, 0 );
}

} // namespace Volition
