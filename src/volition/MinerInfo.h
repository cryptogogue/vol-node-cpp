// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINERINFO_H
#define VOLITION_MINERINFO_H

#include <volition/common.h>
#include <volition/CryptoKey.h>

namespace Volition {

//================================================================//
// MinerInfo
//================================================================//
class MinerInfo {
private:

    string                  mMinerID;
    string                  mURL;
    CryptoKey               mPublicKey;

public:

    //----------------------------------------------------------------//
    string                  getMinerID          () const;
    const CryptoKey&        getPublicKey        () const;
    string                  getURL              () const;
                            MinerInfo           ();
                            MinerInfo           ( string minerID, string url, const CryptoKey& publicKey );
                            MinerInfo           ( const MinerInfo& minerInfo );
                            ~MinerInfo          ();
};

} // namespace Volition
#endif
