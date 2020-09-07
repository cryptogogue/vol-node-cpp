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
        TYPE_EC         = 0x01,
        TYPE_RSA        = 0x02,
        TYPE_MASK       = 0x0f,
    };
    
    enum Encoding {
        ENCODING_HEX    = 0x10,
        ENCODING_PEM    = 0x20,
        ENCODING_MASK   = 0xf0,
    };
    
    enum EncodeAs {
        ENCODE_AS_ANY,
        ENCODE_AS_HEX,
        ENCODE_AS_PEM,
        ENCODE_AS_KEY_ID,
    };
    
    enum Format {
        EC_HEX          = TYPE_EC | ENCODING_HEX,
        EC_PEM          = TYPE_EC | ENCODING_PEM,
        RSA_HEX         = TYPE_RSA | ENCODING_HEX,
        RSA_PEM         = TYPE_RSA | ENCODING_PEM,
        UNKNOWN         = 0,
    };

private:

    //----------------------------------------------------------------//
    static void         dumpRSAHex              ( const CryptoKey& cryptoKey );
    static Format       getFormatFromString     ( string format );
    static string       getStringFromFormat     ( Format format );
    void                initAsPEM               ( const CryptoKey& cryptoKey );
    void                initFromEC              ( const CryptoKey& cryptoKey, EncodeAs encodeAs );
    void                initFromRSA             ( const CryptoKey& cryptoKey, EncodeAs encodeAs );
    KeyPairPtr          makeKeyPairEC           () const;
    KeyPairPtr          makeKeyPairRSA          () const;
    static string       openSSLString           ( char* c );

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
                        CryptoKeyInfo           ( const CryptoKey& cryptoKey, EncodeAs encodeAs = ENCODE_AS_ANY );
    static string       getKeyID                ( const CryptoKey& cryptoKey );
    KeyPairPtr          makeKeyPair             () const;
};

} // namespace Volition
#endif
