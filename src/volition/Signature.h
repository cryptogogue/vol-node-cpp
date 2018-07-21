// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIGNATURE_H
#define VOLITION_SIGNATURE_H

#include <volition/common.h>
#include <volition/CryptoKey.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// Signature
//================================================================//
class Signature :
    public AbstractSerializable {
private:

    Poco::DigestEngine::Digest              mDigest;
    Poco::DigestEngine::Digest              mSignature;
    string                                  mHashAlgorithm;

protected:

    //----------------------------------------------------------------//
    static void                             digest              ( string str, Poco::Crypto::ECDSADigestEngine& digestEngine );
    static void                             digest              ( AbstractSerializable& serializable, Poco::Crypto::ECDSADigestEngine& digestEngine );

    //----------------------------------------------------------------//
    void                                    AbstractSerializable_serialize      ( AbstractSerializer& serializer ) override;

public:

    static constexpr const char* DEFAULT_HASH_ALGORITHM = "SHA256";

    //----------------------------------------------------------------//
    const Poco::DigestEngine::Digest&       getDigest           () const;
    string                                  getHashAlgorithm    () const;
    const Poco::DigestEngine::Digest&       getSignature        () const;
    const Poco::DigestEngine::Digest&       sign                ( string str, const CryptoKey& key, string hashAlgorithm = DEFAULT_HASH_ALGORITHM );
    const Poco::DigestEngine::Digest&       sign                ( AbstractSerializable& serializable, const CryptoKey& key, string hashAlgorithm = DEFAULT_HASH_ALGORITHM );
                                            Signature           ();
                                            ~Signature          ();
    static string                           toHex               ( const Poco::DigestEngine::Digest& digest );
    bool                                    verify              ( string str, const CryptoKey& key ) const;
    bool                                    verify              ( AbstractSerializable& serializable, const CryptoKey& key ) const;
};

} // namespace Volition
#endif
