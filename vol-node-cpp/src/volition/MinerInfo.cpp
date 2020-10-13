// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/MinerInfo.h>

namespace Volition {

//================================================================//
// MinerInfo
//================================================================//

//----------------------------------------------------------------//
AccountID MinerInfo::getAccountIndex () const {
    return this->mAccountIndex;
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
const Digest& MinerInfo::getVisage () const {
    return this->mVisage;
}

//----------------------------------------------------------------//
MinerInfo::MinerInfo () {
}

//----------------------------------------------------------------//
MinerInfo::MinerInfo ( AccountID accountIndex, string url, const CryptoKey& publicKey, const Digest& visage ) :
    mAccountIndex ( accountIndex ),
    mURL ( url ),
    mPublicKey ( publicKey ),
    mVisage ( visage ) {
}

//----------------------------------------------------------------//
MinerInfo::MinerInfo ( const MinerInfo& minerInfo ) :
    mAccountIndex ( minerInfo.getAccountIndex ()),
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
