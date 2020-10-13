// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINERINFO_H
#define VOLITION_MINERINFO_H

#include <volition/common.h>
#include <volition/Account.h>
#include <volition/CryptoKey.h>

namespace Volition {

//================================================================//
// MinerInfo
//================================================================//
class MinerInfo :
    public AbstractSerializable {
private:

    AccountID          mAccountIndex;
    string                  mURL;
    CryptoKey               mPublicKey;
    Digest                  mVisage;

public:

    //----------------------------------------------------------------//
    AccountID          getAccountIndex     () const;
    const CryptoKey&        getPublicKey        () const;
    string                  getURL              () const;
    const Digest&           getVisage           () const;
                            MinerInfo           ();
                            MinerInfo           ( AccountID accountIndex, string url, const CryptoKey& publicKey, const Digest& visage );
                            MinerInfo           ( const MinerInfo& minerInfo );
                            ~MinerInfo          ();
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "accountIndex",      this->mAccountIndex );
        serializer.serialize ( "url",               this->mURL );
        serializer.serialize ( "publicKey",         this->mPublicKey );
        serializer.serialize ( "visage",            this->mVisage );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "accountIndex",      this->mAccountIndex );
        serializer.serialize ( "url",               this->mURL );
        serializer.serialize ( "publicKey",         this->mPublicKey );
        serializer.serialize ( "visage",            this->mVisage );
    }
};

} // namespace Volition
#endif
