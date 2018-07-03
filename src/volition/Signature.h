// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIGNATURE_H
#define VOLITION_SIGNATURE_H

#include "common.h"
#include "AbstractHashable.h"
#include "AbstractSerializable.h"

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
    static void                             digest              ( const AbstractHashable& hashable, Poco::Crypto::ECDSADigestEngine& digestEngine );

    //----------------------------------------------------------------//
    void        AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void        AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;

public:

    static constexpr const char* DEFAULT_HASH_ALGORITHM = "SHA256";

    //----------------------------------------------------------------//
    const Poco::DigestEngine::Digest&       getDigest           () const;
    string                                  getHashAlgorithm    () const;
    const Poco::DigestEngine::Digest&       getSignature        () const;
    const Poco::DigestEngine::Digest&       sign                ( string str, const Poco::Crypto::ECKey& key, string hashAlgorithm = DEFAULT_HASH_ALGORITHM );
    const Poco::DigestEngine::Digest&       sign                ( const AbstractHashable& hashable, const Poco::Crypto::ECKey& key, string hashAlgorithm = DEFAULT_HASH_ALGORITHM );
                                            Signature           ();
                                            ~Signature          ();
    static string                           toHex               ( const Poco::DigestEngine::Digest& digest );
    bool                                    verify              ( string str, const Poco::Crypto::ECKey& key ) const;
    bool                                    verify              ( const AbstractHashable& hashable, const Poco::Crypto::ECKey& key ) const;
};

} // namespace Volition
#endif
