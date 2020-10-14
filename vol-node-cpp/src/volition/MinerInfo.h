// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINERINFO_H
#define VOLITION_MINERINFO_H

#include <volition/common.h>
#include <volition/Accessors.h>
#include <volition/Account.h>
#include <volition/CryptoKey.h>

namespace Volition {

//================================================================//
// MinerInfo
//================================================================//
class MinerInfo :
    public AbstractSerializable {
private:

    string                  mURL;
    CryptoKey               mPublicKey;
    string                  mMotto;
    Signature               mVisage;

public:

    GET ( string,               Motto,              mMotto )
    GET ( const CryptoKey&,     PublicKey,          mPublicKey )
    GET ( string,               URL,                mURL )
    GET ( const Signature&,     Visage,             mVisage )

    //----------------------------------------------------------------//
    bool                    isValid             () const;
                            MinerInfo           ();
                            MinerInfo           ( string url, const CryptoKey& publicKey, string motto, const Signature& visage );
                            MinerInfo           ( const MinerInfo& minerInfo );
                            ~MinerInfo          ();
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "url",           this->mURL );
        serializer.serialize ( "key",           this->mPublicKey );
        serializer.serialize ( "motto",         this->mMotto );
        serializer.serialize ( "visage",        this->mVisage );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "url",           this->mURL );
        serializer.serialize ( "key",           this->mPublicKey.getPublicKey ());
        serializer.serialize ( "motto",         this->mMotto );
        serializer.serialize ( "visage",        this->mVisage );
    }
};

} // namespace Volition
#endif
