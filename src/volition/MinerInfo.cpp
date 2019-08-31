// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/MinerInfo.h>

namespace Volition {

//================================================================//
// MinerInfo
//================================================================//

//----------------------------------------------------------------//
Account::Index MinerInfo::getAccountIndex () const {
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
MinerInfo::MinerInfo () {
}

//----------------------------------------------------------------//
MinerInfo::MinerInfo ( Account::Index accountIndex, string url, const CryptoKey& publicKey ) :
    mAccountIndex ( accountIndex ),
    mURL ( url ),
    mPublicKey ( publicKey ) {
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
