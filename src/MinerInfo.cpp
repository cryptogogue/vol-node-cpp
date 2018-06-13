// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "AbstractHashable.h"
#include "MinerInfo.h"

namespace Volition {

//================================================================//
// MinerInfo
//================================================================//

//----------------------------------------------------------------//
string MinerInfo::getMinerID () const {
    return this->mMinerID;
}

//----------------------------------------------------------------//
const Poco::Crypto::ECKey& MinerInfo::getPublicKey () const {
    return this->mPublicKey;
}

//----------------------------------------------------------------//
string MinerInfo::getURL () const {
    return this->mURL;
}

//----------------------------------------------------------------//
MinerInfo::MinerInfo ( string minerID, string url, const Poco::Crypto::ECKey& publicKey ) :
    mMinerID ( minerID ),
    mURL ( url ),
    mPublicKey ( publicKey ) {
}

//----------------------------------------------------------------//
MinerInfo::MinerInfo ( const MinerInfo& minerInfo ) :
    mMinerID ( minerInfo.getMinerID ()),
    mURL ( minerInfo.getURL ()),
    mPublicKey ( minerInfo.getPublicKey ()) {
}

//----------------------------------------------------------------//
MinerInfo::~MinerInfo () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
