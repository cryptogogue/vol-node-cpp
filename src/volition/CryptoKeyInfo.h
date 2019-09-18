// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CRYPTOKEYINFO_H
#define VOLITION_CRYPTOKEYINFO_H

#include <volition/common.h>
#include <volition/Signature.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

class CryptoKey;

//================================================================//
// CryptoKeyInfo
//================================================================//
class CryptoKeyInfo :
    public AbstractSerializable {
public:

    typedef shared_ptr < Poco::Crypto::KeyPair > KeyPairPtr;

    enum Type {
        EC              = 0x01,
        RSA             = 0x02,
        TYPE_MASK       = 0x0f,
    };
    
    enum Encoding {
        HEX             = 0x10,
        PEM             = 0x20,
        ENCODING_MASK   = 0xf0,
    };
    
    enum Format {
        EC_HEX          = EC | HEX,
        EC_PEM          = EC | PEM,
        RSA_HEX         = RSA | HEX,
        RSA_PEM         = RSA | PEM,
        UNKNOWN         = 0,
    };

private:

    //----------------------------------------------------------------//
    static Format       getFormatFromString     ( string format );
    static string       getStringFromFormat     ( Format format );
    void                initFromEC              ( const CryptoKey& cryptoKey, Encoding encoding );
    void                initFromRSA             ( const CryptoKey& cryptoKey, Encoding encoding );
    KeyPairPtr          makeKeyPairEC           () const;
    KeyPairPtr          makeKeyPairRSA          () const;

    //----------------------------------------------------------------//
    void                AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    Format              mFormat;
    string              mGroupName;
    string              mPublicKey;
    string              mPrivateKey;

    //----------------------------------------------------------------//
    operator bool () const {
        return this->mFormat != UNKNOWN;
    }

    //----------------------------------------------------------------//
                        CryptoKeyInfo           ();
                        CryptoKeyInfo           ( const CryptoKey& cryptoKey, Encoding encoding = HEX );
    KeyPairPtr          makeKeyPair             () const;
};

} // namespace Volition
#endif
