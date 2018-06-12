// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTSIGNABLE_H
#define VOLITION_ABSTRACTSIGNABLE_H

#include "common.h"
#include "AbstractHashable.h"
#include "AbstractSerializable.h"

namespace Volition {

//================================================================//
// Signable
//================================================================//
class Signable :
    public AbstractHashable,
    public AbstractSerializable {
private:

    Poco::DigestEngine::Digest              mDigest;
    Poco::DigestEngine::Digest              mSignature;
    unique_ptr < Poco::Crypto::ECKey >      mPublicKey;
    string                                  mHashAlgorithm;

protected:

    //----------------------------------------------------------------//
    virtual const Poco::DigestEngine::Digest&       Signable_sign               ( const Poco::Crypto::ECKey& key, string hashAlgorithm );
    virtual bool                                    Signable_verify             ( const Poco::Crypto::ECKey& key, string hashAlgorithm ) const;

    //----------------------------------------------------------------//
    void        AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void        AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;

public:

    static constexpr const char* DEFAULT_HASH_ALGORITHM = "SHA256";

    //----------------------------------------------------------------//
    const Poco::DigestEngine::Digest&       getDigest           () const;
    string                                  getHashAlgorithm    () const;
    const Poco::Crypto::ECKey*              getPublicKey        () const;
    const Poco::DigestEngine::Digest&       getSignature        () const;
    const Poco::DigestEngine::Digest&       sign                ( const Poco::Crypto::ECKey& key, string hashAlgorithm = DEFAULT_HASH_ALGORITHM );
                                            Signable            ();
                                            ~Signable           ();
    static string                           toHex               ( const Poco::DigestEngine::Digest& digest );
    bool                                    verify              () const;
};

} // namespace Volition
#endif
