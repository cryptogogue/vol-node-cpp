// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/TheContext.h>

namespace Volition {

static const char* PUBLIC_KEY_PEM       = "-----BEGIN PUBLIC KEY-----\nMFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAEzfUD/qtmfa4H2NJ+vriC8SNm0bUe8dMx\nBlOig2FzzOZE1iVu3u1Tgvvr4MsYmY2KYxOt+zHWcT5DlSKmrmaQqw==\n-----END PUBLIC KEY-----\n";
static const char* DIGEST_STRING        = "483b57365c189d2877a0c2afa9d5238831190d45584a57e1f98abc7173d64892";

//================================================================//
// TheContext
//================================================================//

//----------------------------------------------------------------//
const Digest& TheContext::getGenesisBlockDigest () const {

    return this->mDigest;
}

//----------------------------------------------------------------//
const CryptoKey& TheContext::getGenesisBlockKey () const {

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
    
    this->mDigest = DIGEST_STRING;
}

//----------------------------------------------------------------//
void TheContext::setGenesisBlockDigest ( const Digest& digest ) {

    this->mDigest = digest;
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
