//
//  main.cpp
// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Genesis.h>

namespace Volition {

static const char* PUBLIC_KEY_STRING    = "-----BEGIN PUBLIC KEY-----\nMFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAEzfUD/qtmfa4H2NJ+vriC8SNm0bUe8dMx\nBlOig2FzzOZE1iVu3u1Tgvvr4MsYmY2KYxOt+zHWcT5DlSKmrmaQqw==\n-----END PUBLIC KEY-----\n";
static const char* DIGEST_STRING        = "caf28a0415ff5fc3edf6a87ff5f0f11fe8b4bbb1bc6343a7b715887325647167";

//================================================================//
// Genesis
//================================================================//

const char* Genesis::EC_CURVE = "secp256k1";

//----------------------------------------------------------------//
const Poco::DigestEngine::Digest& Genesis::getDigest () const {

    assert ( this->mDigest );
    return *this->mDigest;
}

//----------------------------------------------------------------//
const Poco::Crypto::ECKey& Genesis::getKey () const {

    assert ( this->mKey );
    return *this->mKey;
}

//----------------------------------------------------------------//
Genesis::Genesis () {

    stringstream genesisKeyStream ( PUBLIC_KEY_STRING );
    this->mKey = make_unique < Poco::Crypto::ECKey >( &genesisKeyStream );
    this->mDigest = make_unique < Poco::DigestEngine::Digest >( Poco::DigestEngine::digestFromHex ( DIGEST_STRING ));
}

//----------------------------------------------------------------//
void Genesis::setDigest ( const Poco::DigestEngine::Digest& digest ) {

    this->mDigest = make_unique < Poco::DigestEngine::Digest >( digest );
}

//----------------------------------------------------------------//
void Genesis::setKey ( const Poco::Crypto::ECKey& key ) {

    this->mKey = make_unique < Poco::Crypto::ECKey >( key );
}

} // namespace Volition
