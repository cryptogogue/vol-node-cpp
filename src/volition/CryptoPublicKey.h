// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CRYPTOPUBLICKEY_H
#define VOLITION_CRYPTOPUBLICKEY_H

#include <volition/common.h>
#include <volition/AbstractCryptoKey.h>
#include <volition/CryptoKeyInfo.h>
#include <volition/Signature.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// CryptoPublicKey
//================================================================//
class CryptoPublicKey :
    public AbstractSerializable,
    public AbstractCryptoKey {
protected:
    
    shared_ptr < Poco::Crypto::KeyPair >      mKeyPair;
    
    //----------------------------------------------------------------//
    KeyPairPtr          AbstractCryptoKey_getKeyPair            () const override;
    void                AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
                        CryptoPublicKey         ();
                        CryptoPublicKey         ( CryptoKeyInfo keyInfo );
                        ~CryptoPublicKey        ();
    void                load                    ( string filename );
    void                rsaFromPEM              ( string publicKey );
    string              toPEM                   () const;
};

} // namespace Volition
#endif
