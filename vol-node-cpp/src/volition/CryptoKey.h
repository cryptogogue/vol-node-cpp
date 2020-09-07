// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CRYPTOKEY_H
#define VOLITION_CRYPTOKEY_H

#include <volition/common.h>
#include <volition/Signature.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

class DigestEngine;

//================================================================//
// CryptoKey
//================================================================//
class CryptoKey :
    public AbstractSerializable {
private:

    shared_ptr < Poco::Crypto::KeyPair >      mKeyPair;
    
    //----------------------------------------------------------------//
    void            AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void            AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    enum : unsigned long {
        RSA_EXP_3       = 0x3L,
        RSA_EXP_65537   = 0x10001L,
    };

    enum : uint {
        RSA_512         = 512,
        RSA_1024        = 1024,
        RSA_2048        = 2048,
        RSA_4096        = 4096,
    };

    static const char*  DEFAULT_EC_GROUP_NAME;
    static const int    DEFAULT_EC_GROUP_NID = NID_secp256k1;

    //----------------------------------------------------------------//
                        CryptoKey               ();
                        ~CryptoKey              ();
    void                elliptic                ( int nid = DEFAULT_EC_GROUP_NID );
    void                elliptic                ( string groupName );
    static string       getGroupNameFromNID     ( int nid );
    string              getKeyID                () const; // this is just a simple hash of the hex-encoded public key
    static int          getNIDFromGroupName     ( string groupName );
    static bool         hasCurve                ( int nid );
    static bool         hasCurve                ( string groupName );
    void                rsa                     ( uint keyLength = RSA_1024, unsigned long exp = RSA_EXP_65537 );
    Signature           sign                    ( const Digest& digest, string hashAlgorithm = Digest::DEFAULT_HASH_ALGORITHM ) const;
    Signature           sign                    ( const Digest::DigestFunc& digestFunc, string hashAlgorithm = Digest::DEFAULT_HASH_ALGORITHM ) const;
    Signature           sign                    ( const AbstractSerializable& serializable, string hashAlgorithm = Digest::DEFAULT_HASH_ALGORITHM ) const;
    Signature           sign                    ( string message, string hashAlgorithm = Digest::DEFAULT_HASH_ALGORITHM ) const;
    bool                verify                  ( const Signature& signature, const Digest& digest ) const;
    bool                verify                  ( const Signature& signature, const Digest::DigestFunc& digestFunc ) const;
    bool                verify                  ( const Signature& signature, const AbstractSerializable& serializable ) const;
    bool                verify                  ( const Signature& signature, string message ) const;
    
    //----------------------------------------------------------------//
    operator bool () const {
        return this->mKeyPair ? true : false;
    }
    
    //----------------------------------------------------------------//
    operator const Poco::Crypto::ECKey* () const {
        Poco::Crypto::ECKey* ecKey = dynamic_cast < Poco::Crypto::ECKey* >( this->mKeyPair.get ());
        return ecKey;
    }
    
    //----------------------------------------------------------------//
    operator const Poco::Crypto::ECKey& () const {
        const Poco::Crypto::ECKey* ecKey = *this;
        assert ( ecKey );
        return *ecKey;
    }
    
    //----------------------------------------------------------------//
    operator const Poco::Crypto::RSAKey* () const {
        Poco::Crypto::RSAKey* rsaKey = dynamic_cast < Poco::Crypto::RSAKey* >( this->mKeyPair.get ());
        return rsaKey;
    }
    
    //----------------------------------------------------------------//
    operator const Poco::Crypto::RSAKey& () const {
        const Poco::Crypto::RSAKey* rsaKey = *this;
        assert ( rsaKey );
        return *rsaKey;
    }
    
    //----------------------------------------------------------------//
    operator const Poco::Crypto::KeyPair* () const {
        return this->mKeyPair.get ();
    }
};

} // namespace Volition
#endif
