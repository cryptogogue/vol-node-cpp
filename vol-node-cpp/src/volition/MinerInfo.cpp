// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/MinerInfo.h>

namespace Volition {

//================================================================//
// MinerInfo
//================================================================//

//----------------------------------------------------------------//
bool MinerInfo::isValid () const {

    return (( this->mPublicKey.getType () == CryptoKeyInfo::TYPE_RSA ) && this->mPublicKey.verify ( this->mVisage, this->mMotto ));
}

//----------------------------------------------------------------//
MinerInfo::MinerInfo () {
}

//----------------------------------------------------------------//
MinerInfo::MinerInfo ( string url, const CryptoKey& publicKey, string motto, const Signature& visage ) :
    mURL ( url ),
    mPublicKey ( publicKey.getPublicKey ()),
    mMotto ( motto ),
    mVisage ( visage ) {
}

//----------------------------------------------------------------//
MinerInfo::MinerInfo ( const MinerInfo& minerInfo ) :
    mURL ( minerInfo.getURL ()),
    mPublicKey ( minerInfo.getPublicKey ()),
    mMotto ( minerInfo.getMotto ()),
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
