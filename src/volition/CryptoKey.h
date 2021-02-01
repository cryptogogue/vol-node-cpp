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

    typedef std::function < void ( Poco::DigestOutputStream& )> DigestFunc;

    shared_ptr < Poco::Crypto::KeyPair >      mKeyPair;
    
    //----------------------------------------------------------------//
    void            AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void            AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

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
    Signature           sign                    ( const DigestFunc& digestFunc, string hashAlgorithm = Digest::DEFAULT_HASH_ALGORITHM ) const;
    Signature           sign                    ( const AbstractSerializable& serializable, string hashAlgorithm = Digest::DEFAULT_HASH_ALGORITHM ) const;
    Signature           sign                    ( string message, string hashAlgorithm = Digest::DEFAULT_HASH_ALGORITHM ) const;
    bool                verify                  ( const Signature& signature, const DigestFunc& digestFunc ) const;
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
    operator const Poco::Crypto::KeyPair* () const {
        return this->mKeyPair.get ();
    }
};

} // namespace Volition
#endif
