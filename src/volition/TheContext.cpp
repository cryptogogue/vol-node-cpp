//
//  main.cpp
// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/TheContext.h>

namespace Volition {

static const char* PUBLIC_KEY_PEM       = "-----BEGIN PUBLIC KEY-----\nMFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAEzfUD/qtmfa4H2NJ+vriC8SNm0bUe8dMx\nBlOig2FzzOZE1iVu3u1Tgvvr4MsYmY2KYxOt+zHWcT5DlSKmrmaQqw==\n-----END PUBLIC KEY-----\n";
static const char* DIGEST_STRING        = "21502265b201586201e75ed45828b748187c2a370a10dca772692f5656564273";

//================================================================//
// TheContext
//================================================================//

//----------------------------------------------------------------//
const Poco::DigestEngine::Digest& TheContext::getGenesisBlockDigest () const {

    assert ( this->mDigest );
    return *this->mDigest;
}

//----------------------------------------------------------------//
const CryptoKey& TheContext::getGenesisBlockKey () const {

    assert ( this->mKey );
    return this->mKey;
}

//----------------------------------------------------------------//
TheContext::ScoringMode TheContext::getScoringMode () const {

    return this->mScoringMode;
}

//----------------------------------------------------------------//
TheContext::TheContext () :
    mScoringMode ( ScoringMode::ALLURE ) {

    Poco::JSON::Object::Ptr object = new Poco::JSON::Object ();
    object->set ( "type", "EC_PEM" );
    object->set ( "publicKey", PUBLIC_KEY_PEM );

    FromJSONSerializer::fromJSON ( this->mKey, *object );
    
    this->mDigest = make_unique < Poco::DigestEngine::Digest >( Poco::DigestEngine::digestFromHex ( DIGEST_STRING ));
}

//----------------------------------------------------------------//
void TheContext::setGenesisBlockDigest ( const Poco::DigestEngine::Digest& digest ) {

    this->mDigest = make_unique < Poco::DigestEngine::Digest >( digest );
}

//----------------------------------------------------------------//
void TheContext::setGenesisBlockKey ( const CryptoKey& key ) {

    this->mKey = key;
}

//----------------------------------------------------------------//
void TheContext::setScoringMode ( ScoringMode scoringMode ) {

    this->mScoringMode = scoringMode;
}

} // namespace Volition
