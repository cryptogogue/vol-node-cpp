// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTCRYPTOKEY_H
#define VOLITION_ABSTRACTCRYPTOKEY_H

#include <volition/common.h>
#include <volition/CryptoKeyInfo.h>
#include <volition/Signature.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// AbstractCryptoKey
//================================================================//
class AbstractCryptoKey {
protected:

    typedef const Poco::Crypto::KeyPair* KeyPairPtr;

    //----------------------------------------------------------------//
    virtual KeyPairPtr      AbstractCryptoKey_getKeyPair        () const = 0;

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
    operator bool () const {
        return this->getKeyPair () ? true : false;
    }

    //----------------------------------------------------------------//
                                AbstractCryptoKey           ();
    virtual                     ~AbstractCryptoKey          ();
    static string               getGroupNameFromNID         ( int nid );
    string                      getKeyID                    () const; // this is just a simple hash of the hex-encoded public key
    KeyPairPtr                  getKeyPair                  () const;
    static int                  getNIDFromGroupName         ( string groupName );
    CryptoKeyInfo::Type         getType                     () const;
    static bool                 hasCurve                    ( int nid );
    static bool                 hasCurve                    ( string groupName );
    bool                        verify                      ( const Signature& signature, const Digest& digest ) const;
    bool                        verify                      ( const Signature& signature, const Digest::DigestFunc& digestFunc ) const;
    bool                        verify                      ( const Signature& signature, const AbstractSerializable& serializable ) const;
    bool                        verify                      ( const Signature& signature, string message ) const;
};

} // namespace Volition
#endif
