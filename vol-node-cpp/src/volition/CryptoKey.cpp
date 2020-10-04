// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/CryptoKey.h>
#include <volition/CryptoKeyInfo.h>
#include <volition/FNV1a.h>
#include <volition/Format.h>

#include <openssl/ecdsa.h>
#include <openssl/rsa.h>

// bitaddress.org
// private: KzvyiCGoxXMaraHf3HmtZsttox2U99VTPq9RTWyJqsTVdEZ9nYDV
// public compressed: 0322FDB3D240158274B8FCA64263B04215A55B6F248AEB51DDED78F052C2A73DC1
// public: 0422FDB3D240158274B8FCA64263B04215A55B6F248AEB51DDED78F052C2A73DC135CB34215CFC80ECCAF0FE74982DDBA6A85F629C71D0BEB4434E4F7DEA1E623D

namespace Volition {

//================================================================//
// CryptoKey
//================================================================//

const char* CryptoKey::DEFAULT_EC_GROUP_NAME = "secp256k1";

//----------------------------------------------------------------//
CryptoKey::CryptoKey () {
}

//----------------------------------------------------------------//
CryptoKey::~CryptoKey () {
}

//----------------------------------------------------------------//
void CryptoKey::elliptic ( int nid ) {

    this->elliptic ( CryptoKey::getGroupNameFromNID ( nid ));
}

//----------------------------------------------------------------//
void CryptoKey::elliptic ( string groupName ) {

    this->mKeyPair = make_shared < Poco::Crypto::ECKey >( groupName );
}

//----------------------------------------------------------------//
string CryptoKey::getGroupNameFromNID ( int nid ) {

    return Poco::Crypto::ECKey::getCurveName ( nid );
}

//----------------------------------------------------------------//
string CryptoKey::getKeyID () const {

    return CryptoKeyInfo::getKeyID ( *this );
}

//----------------------------------------------------------------//
int CryptoKey::getNIDFromGroupName ( string groupName ) {

    return Poco::Crypto::ECKey::getCurveNID ( groupName );
}

//----------------------------------------------------------------//
bool CryptoKey::hasCurve ( int nid ) {

    return CryptoKey::hasCurve ( CryptoKey::getGroupNameFromNID ( nid ));
}

//----------------------------------------------------------------//
bool CryptoKey::hasCurve ( string groupName ) {

    return Poco::Crypto::ECKey::hasCurve ( groupName );
}

//----------------------------------------------------------------//
void CryptoKey::rsa ( uint keyLength, unsigned long exp ) {
    
    RSA* rsaKey = RSA_new ();
    int ret = 0;
    BIGNUM* bn = 0;
    try {
        bn = BN_new ();
        BN_set_word ( bn, exp );
        ret = RSA_generate_key_ex ( rsaKey, ( int )keyLength, bn, 0 );
        BN_free ( bn );
    }
    catch ( ... ) {
        BN_free ( bn );
        throw;
    }
    
    // convert to Poco key
    EVP_PKEY* pkey = EVP_PKEY_new ();
    assert ( pkey );
    
    EVP_PKEY_set1_RSA ( pkey, rsaKey );
    RSA_free ( rsaKey );

    this->mKeyPair = make_shared < Poco::Crypto::RSAKey >( Poco::Crypto::EVPPKey { pkey }); // prevent the 'most vexing parse'

    EVP_PKEY_free ( pkey );
}

//----------------------------------------------------------------//
void CryptoKey::rsaFromPEM ( string publicKey, string privateKey ) {

    // awkward, but DRY with CryptoKeyInfo.
    
    CryptoKeyInfo info;
    info.mFormat        = CryptoKeyInfo::RSA_PEM;
    info.mPublicKey     = publicKey;
    info.mPrivateKey    = privateKey;

    // TODO: passphrase
    this->mKeyPair = info.makeKeyPair ();
}

//----------------------------------------------------------------//
Signature CryptoKey::sign ( const Digest& digest, string hashAlgorithm ) const {

    Digest sig;

    int nid = Digest::nid ( hashAlgorithm );
    assert ( nid );

    if ( this->mKeyPair ) {
        switch ( this->mKeyPair->type ()) {

            case Poco::Crypto::KeyPair::KT_EC: {

                const Poco::Crypto::ECKey* pocoECKey = *this;
                assert ( pocoECKey );

                EC_KEY* pKey = pocoECKey->impl ()->getECKey ();
                assert ( pKey );

                uint sigLen = ( unsigned int )ECDSA_size ( pKey );

                sig.resize ( sigLen );

                int result = ECDSA_sign (
                    nid,
                    &digest [ 0 ],
                    ( int )digest.size (),
                    &sig [ 0 ],
                    &sigLen,
                    pKey
                );
                assert ( result == 1 );

                if ( sigLen < sig.size ()) {
                    sig.resize ( sigLen ); // EC signature actual length may be shorter than max returned by ECDSA_size ().
                }
                break;
            }

            case Poco::Crypto::KeyPair::KT_RSA: {
                
                const Poco::Crypto::RSAKey* pocoRSAKey = *this;
                assert ( pocoRSAKey );

                RSA* pKey = pocoRSAKey->impl ()->getRSA ();
                assert ( pKey );

                uint sigLen = ( unsigned int )RSA_size ( pKey );

                sig.resize ( sigLen );

                int result = RSA_sign (
                    nid,
                    &digest [ 0 ],
                    ( unsigned int )digest.size (),
                    &sig [ 0 ],
                    &sigLen,
                    pKey
                );
                assert ( result == 1 );

                break;
            }
        }
    }
    return Signature ( sig, hashAlgorithm );
}

//----------------------------------------------------------------//
Signature CryptoKey::sign ( const Digest::DigestFunc& digestFunc, string hashAlgorithm ) const {
    
    Digest digest ( digestFunc, hashAlgorithm );
    return this->sign ( digest, hashAlgorithm );
}

//----------------------------------------------------------------//
Signature CryptoKey::sign ( const AbstractSerializable& serializable, string hashAlgorithm ) const {

    return this->sign (
        [ & ]( Poco::DigestOutputStream& stream ) {
            ToJSONSerializer::toDigest ( serializable, stream );
        },
        hashAlgorithm
    );
}

//----------------------------------------------------------------//
Signature CryptoKey::sign ( string message, string hashAlgorithm ) const {

    return this->sign (
        [ = ]( Poco::DigestOutputStream& stream ) {
            stream << message;
        },
        hashAlgorithm
    );
}

//----------------------------------------------------------------//
string CryptoKey::toPEM () const {

    CryptoKeyInfo info ( *this, CryptoKeyInfo::ENCODE_AS_PEM );
    return info.mPrivateKey.size () ? info.mPrivateKey : info.mPublicKey;
}

//----------------------------------------------------------------//
bool CryptoKey::verify ( const Signature& signature, const Digest& digest ) const {

    bool result = false;

    int nid = Digest::nid ( signature.getHashAlgorithm ());
    assert ( nid );

    if ( this->mKeyPair && signature ) {
        switch ( this->mKeyPair->type ()) {
                
            case Poco::Crypto::KeyPair::KT_EC: {
                
                const Poco::Crypto::ECKey* pocoECKey = *this;
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
            
                const Poco::Crypto::RSAKey* pocoRSAKey = *this;
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
bool CryptoKey::verify ( const Signature& signature, const Digest::DigestFunc& digestFunc ) const {

    Digest digest ( digestFunc, signature.getHashAlgorithm ());
    return this->verify ( signature, digest );
}

//----------------------------------------------------------------//
bool CryptoKey::verify ( const Signature& signature, const AbstractSerializable& serializable ) const {

    return this->verify ( signature, [ & ]( Poco::DigestOutputStream& stream ) {
        ToJSONSerializer::toDigest ( serializable, stream );
    });
}

//----------------------------------------------------------------//
bool CryptoKey::verify ( const Signature& signature, string message ) const {

    return this->verify ( signature, [ = ]( Poco::DigestOutputStream& stream ) {
        stream << message;
    });
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void CryptoKey::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    CryptoKeyInfo info;
    info.serializeFrom ( serializer );
    this->mKeyPair = info.makeKeyPair ();
}

//----------------------------------------------------------------//
void CryptoKey::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    CryptoKeyInfo info ( *this );
    info.serializeTo ( serializer );
}

} // namespace Volition
