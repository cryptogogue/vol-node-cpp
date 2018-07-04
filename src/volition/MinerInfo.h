// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINERINFO_H
#define VOLITION_MINERINFO_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// MinerInfo
//================================================================//
class MinerInfo {
private:

    string                  mMinerID;
    string                  mURL;
    Poco::Crypto::ECKey     mPublicKey;

public:

    //----------------------------------------------------------------//
    string                          getMinerID      () const;
    const Poco::Crypto::ECKey&      getPublicKey    () const;
    string                          getURL          () const;
                                    MinerInfo       ( string minerID, string url, const Poco::Crypto::ECKey& publicKey );
                                    MinerInfo       ( const MinerInfo& minerInfo );
                                    ~MinerInfo      ();
};

} // namespace Volition
#endif
