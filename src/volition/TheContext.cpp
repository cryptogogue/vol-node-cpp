// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/TheContext.h>

namespace Volition {

//static const string PUBLIC_KEY_PEM        = "-----BEGIN PUBLIC KEY-----\nMFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAEzfUD/qtmfa4H2NJ+vriC8SNm0bUe8dMx\nBlOig2FzzOZE1iVu3u1Tgvvr4MsYmY2KYxOt+zHWcT5DlSKmrmaQqw==\n-----END PUBLIC KEY-----\n";
//static const string char* DIGEST_STRING   = "9a67ce9a8ccbe4e8491f0c572da5aa390ae636f53403f6fce08943e710998654";

static const string PUBLIC_KEY_PEM;
static const string DIGEST_STRING;

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
size_t TheContext::getScoringModulo () const {

    return this->mScoringModulo;
}

//----------------------------------------------------------------//
double TheContext::getWindow () const {

    return this->mRewriteWindowInSeconds;
}

//----------------------------------------------------------------//
bool TheContext::hasGenesisBlockDigest () const {
    return ( DIGEST_STRING.size () > 0 );
}

//----------------------------------------------------------------//
bool TheContext::hasGenesisBlockKey () const {
     return ( PUBLIC_KEY_PEM.size () > 0 );
}

//----------------------------------------------------------------//
TheContext::TheContext () :
    mScoringMode ( ScoringMode::ALLURE ),
    mScoringModulo ( 0 ),
    mRewriteWindowInSeconds ( 0 ) {

    if ( TheContext::hasGenesisBlockKey ()) {
        Poco::JSON::Object::Ptr object = new Poco::JSON::Object ();
        object->set ( "type", "EC_PEM" );
        object->set ( "publicKey", PUBLIC_KEY_PEM );

        FromJSONSerializer::fromJSON ( this->mKey, *object );
    }
    
    if ( TheContext::hasGenesisBlockDigest ()) {
        this->mDigest = DIGEST_STRING;
    }
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
void TheContext::setScoringMode ( ScoringMode scoringMode, size_t modulo ) {

    this->mScoringMode = scoringMode;
    this->mScoringModulo = modulo;
}

//----------------------------------------------------------------//
void TheContext::setWindow ( double window ) {

    this->mRewriteWindowInSeconds = window;
}

} // namespace Volition
