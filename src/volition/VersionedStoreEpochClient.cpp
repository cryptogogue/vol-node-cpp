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
void VersionedStoreEpochClient::setEpoch ( shared_ptr < VersionedStoreEpoch > epoch ) {

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
}

//----------------------------------------------------------------//
VersionedStoreEpochClient::VersionedStoreEpochClient () :
    mVersion ( 0 ) {
}

//----------------------------------------------------------------//
VersionedStoreEpochClient::~VersionedStoreEpochClient () {

    this->setEpoch ( NULL );
}

} // namespace Volition
