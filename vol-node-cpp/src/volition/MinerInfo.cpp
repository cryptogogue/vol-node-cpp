// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/MinerInfo.h>

namespace Volition {

//================================================================//
// MinerInfo
//================================================================//

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
MinerInfo::MinerInfo ( string url, const CryptoKey& publicKey, const Digest& visage ) :
    mURL ( url ),
    mPublicKey ( publicKey ),
    mVisage ( visage ) {
}

//----------------------------------------------------------------//
MinerInfo::MinerInfo ( const MinerInfo& minerInfo ) :
    mURL ( minerInfo.getURL ()),
    mPublicKey ( minerInfo.getPublicKey ()),
    mVisage ( minerInfo.getVisage ()) {
}

//----------------------------------------------------------------//
MinerInfo::~MinerInfo () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
