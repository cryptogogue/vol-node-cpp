// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractCryptoKey.h>
#include <volition/CryptoKeyInfo.h>
#include <volition/FNV1a.h>
#include <volition/Format.h>

#include <openssl/ecdsa.h>
#include <openssl/rsa.h>

namespace Volition {

//================================================================//
// AbstractCryptoKey
//================================================================//

const char* AbstractCryptoKey::DEFAULT_EC_GROUP_NAME = "secp256k1";

//----------------------------------------------------------------//
AbstractCryptoKey::AbstractCryptoKey () {
}

//----------------------------------------------------------------//
AbstractCryptoKey::~AbstractCryptoKey () {
}

//----------------------------------------------------------------//
string AbstractCryptoKey::getGroupNameFromNID ( int nid ) {

    return Poco::Crypto::ECKey::getCurveName ( nid );
}

//----------------------------------------------------------------//
string AbstractCryptoKey::getKeyID () const {

    return CryptoKeyInfo::getKeyID ( this->getKeyPair ());
}

//----------------------------------------------------------------//
const Poco::Crypto::KeyPair* AbstractCryptoKey::getKeyPair () const {

    return this->AbstractCryptoKey_getKeyPair ();
}

//----------------------------------------------------------------//
int AbstractCryptoKey::getNIDFromGroupName ( string groupName ) {

    return Poco::Crypto::ECKey::getCurveNID ( groupName );
}

//----------------------------------------------------------------//
CryptoKeyInfo::Type AbstractCryptoKey::getType () const {

    const Poco::Crypto::KeyPair* keyPair = this->getKeyPair ();

    if ( keyPair ) {
        switch ( keyPair->type ()) {
            case Poco::Crypto::KeyPair::KT_EC:      return CryptoKeyInfo::TYPE_EC;
            case Poco::Crypto::KeyPair::KT_RSA:     return CryptoKeyInfo::TYPE_RSA;
            default:
                break;
        }
    }
    return CryptoKeyInfo::TYPE_UNKNOWN;
}

//----------------------------------------------------------------//
bool AbstractCryptoKey::hasCurve ( int nid ) {

    return AbstractCryptoKey::hasCurve ( AbstractCryptoKey::getGroupNameFromNID ( nid ));
}

//----------------------------------------------------------------//
bool AbstractCryptoKey::hasCurve ( string groupName ) {

    return Poco::Crypto::ECKey::hasCurve ( groupName );
}

//----------------------------------------------------------------//
bool AbstractCryptoKey::verify ( const Signature& signature, const Digest& digest ) const {

    bool result = false;

    int nid = Digest::nid ( signature.getHashAlgorithm ());
    assert ( nid );

    const Poco::Crypto::KeyPair* keyPair = this->getKeyPair ();

    if ( keyPair && signature ) {
        
        switch ( keyPair->type ()) {
                
            case Poco::Crypto::KeyPair::KT_EC: {
                
                const Poco::Crypto::ECKey* pocoECKey = dynamic_cast < const Poco::Crypto::ECKey* >( keyPair );
                assert ( pocoECKey );

                EC_KEY* pKey = pocoECKey->impl ()->getECKey ();
                assert ( pKey );

                Digest sig = signature.getSignature ();

                result = 1 == ECDSA_verify (
                    nid,
                    &digest [ 0 ],
                    ( int )digest.size (),
                    &sig [ 0 ],
                    ( int )sig.size (),
                    pKey
                );
                break;
            }
            
            case Poco::Crypto::KeyPair::KT_RSA: {
            
                const Poco::Crypto::RSAKey* pocoRSAKey = dynamic_cast < const Poco::Crypto::RSAKey* >( keyPair );
                assert ( pocoRSAKey );

                RSA* pKey = pocoRSAKey->impl ()->getRSA ();
                assert ( pKey );

                Digest sig = signature.getSignature ();

                result = 1 == RSA_verify (
                    nid,
                    &digest [ 0 ],
                    ( uint )digest.size (),
                    &sig [ 0 ],
                    ( uint )sig.size (),
                    pKey
                );
                break;
            }
        }
    }
    return result;
}

//----------------------------------------------------------------//
bool AbstractCryptoKey::verify ( const Signature& signature, const Digest::DigestFunc& digestFunc ) const {

    Digest digest ( digestFunc, signature.getHashAlgorithm ());
    return this->verify ( signature, digest );
}

//----------------------------------------------------------------//
bool AbstractCryptoKey::verify ( const Signature& signature, const AbstractSerializable& serializable ) const {

    return this->verify ( signature, [ & ]( Poco::DigestOutputStream& stream ) {
        ToJSONSerializer::toDigest ( serializable, stream );
    });
}

//----------------------------------------------------------------//
bool AbstractCryptoKey::verify ( const Signature& signature, string message ) const {

    return this->verify ( signature, [ = ]( Poco::DigestOutputStream& stream ) {
        stream << message;
    });
}

} // namespace Volition
