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

    Account::Index          mAccountIndex;
    string                  mURL;
    CryptoKey               mPublicKey;

public:

    //----------------------------------------------------------------//
    Account::Index          getAccountIndex     () const;
    const CryptoKey&        getPublicKey        () const;
    string                  getURL              () const;
                            MinerInfo           ();
                            MinerInfo           ( Account::Index accountIndex, string url, const CryptoKey& publicKey );
                            MinerInfo           ( const MinerInfo& minerInfo );
                            ~MinerInfo          ();
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "accountIndex",      this->mAccountIndex );
        serializer.serialize ( "url",               this->mURL );
        serializer.serialize ( "publicKey",         this->mPublicKey );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "accountIndex",      this->mAccountIndex );
        serializer.serialize ( "url",               this->mURL );
        serializer.serialize ( "publicKey",         this->mPublicKey );
    }
};

} // namespace Volition
#endif