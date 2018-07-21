// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/MinerInfo.h>

namespace Volition {

//================================================================//
// MinerInfo
//================================================================//

//----------------------------------------------------------------//
string MinerInfo::getMinerID () const {
    return this->mMinerID;
}

//----------------------------------------------------------------//
const CryptoKey& MinerInfo::getPublicKey () const {
    return this->mPublicKey;
}

//----------------------------------------------------------------//
string MinerInfo::getURL () const {
    return this->mURL;
}

//----------------------------------------------------------------//
MinerInfo::MinerInfo () {
}

//----------------------------------------------------------------//
MinerInfo::MinerInfo ( string minerID, string url, const CryptoKey& publicKey ) :
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
