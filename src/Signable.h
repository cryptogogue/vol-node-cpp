// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTSIGNABLE_H
#define VOLITION_ABSTRACTSIGNABLE_H

#include "common.h"
#include "AbstractHashable.h"

namespace Volition {

//================================================================//
// Signable
//================================================================//
class Signable :
    public AbstractHashable {
private:

    Poco::DigestEngine::Digest              mDigest;
    Poco::DigestEngine::Digest              mSignature;

public:

    //----------------------------------------------------------------//
    const Poco::DigestEngine::Digest&       getDigest           () const;
    const Poco::DigestEngine::Digest&       getSignature        () const;
    const Poco::DigestEngine::Digest&       sign                ( const Poco::Crypto::ECKey& key, string hashAlgorithm );
                                            Signable            ();
                                            ~Signable           ();
    static string                           toHex               ( const Poco::DigestEngine::Digest& digest );
};

} // namespace Volition
#endif
