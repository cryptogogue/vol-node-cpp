// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CRYPTOKEYPAIR_H
#define VOLITION_CRYPTOKEYPAIR_H

#include <volition/common.h>
#include <volition/AbstractCryptoKey.h>
#include <volition/CryptoKeyInfo.h>
#include <volition/CryptoPublicKey.h>
#include <volition/Signature.h>

namespace Volition {

//================================================================//
// CryptoKeyPair
//================================================================//
class CryptoKeyPair :
    public AbstractSerializable,
    public AbstractCryptoKey {
protected:
    
    shared_ptr < Poco::Crypto::KeyPair >    mKeyPair;
    
    //----------------------------------------------------------------//
    KeyPairPtr              AbstractCryptoKey_getKeyPair            () const override;
    void                    AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
                            CryptoKeyPair           ();
                            CryptoKeyPair           ( CryptoKeyInfo keyInfo );
                            ~CryptoKeyPair          ();
    void                    elliptic                ( int nid = DEFAULT_EC_GROUP_NID );
    void                    elliptic                ( string groupName );
    CryptoPublicKey         getPublicKey            () const;
    void                    load                    ( string filename );
    void                    rsa                     ( uint keyLength = RSA_1024, unsigned long exp = RSA_EXP_65537 );
    void                    rsaFromPEM              ( string publicKey, string privateKey );
    Signature               sign                    ( const Digest& digest, string hashAlgorithm = Digest::DEFAULT_HASH_ALGORITHM ) const;
    Signature               sign                    ( const Digest::DigestFunc& digestFunc, string hashAlgorithm = Digest::DEFAULT_HASH_ALGORITHM ) const;
    Signature               sign                    ( const AbstractSerializable& serializable, string hashAlgorithm = Digest::DEFAULT_HASH_ALGORITHM ) const;
    Signature               sign                    ( string message, string hashAlgorithm = Digest::DEFAULT_HASH_ALGORITHM ) const;
    string                  toPEM                   () const;
};

} // namespace Volition
#endif
