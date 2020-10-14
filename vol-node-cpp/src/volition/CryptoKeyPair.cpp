// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/CryptoKeyPair.h>
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
// CryptoPrivateKey
//================================================================//

//----------------------------------------------------------------//
CryptoKeyPair::CryptoKeyPair () {
}

//----------------------------------------------------------------//
CryptoKeyPair::CryptoKeyPair ( CryptoKeyInfo keyInfo ) {

    this->mKeyPair = keyInfo.mPrivateKey.size () ? keyInfo.makeKeyPair () : NULL;
}

//----------------------------------------------------------------//
CryptoKeyPair::~CryptoKeyPair () {
}

//----------------------------------------------------------------//
void CryptoKeyPair::elliptic ( int nid ) {

    this->elliptic ( CryptoKeyPair::getGroupNameFromNID ( nid ));
}

//----------------------------------------------------------------//
void CryptoKeyPair::elliptic ( string groupName ) {

    this->mKeyPair = make_shared < Poco::Crypto::ECKey >( groupName );
}

//----------------------------------------------------------------//
CryptoPublicKey CryptoKeyPair::getPublicKey () const {

    return ( this->mKeyPair ) ? CryptoPublicKey ( CryptoKeyInfo ( this->mKeyPair.get ())) : CryptoPublicKey ();
}

//----------------------------------------------------------------//
void CryptoKeyPair::rsa ( uint keyLength, unsigned long exp ) {
    
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
void CryptoKeyPair::rsaFromPEM ( string publicKey, string privateKey ) {
    
    CryptoKeyInfo info;
    info.mFormat        = CryptoKeyInfo::RSA_PEM;
    info.mPublicKey     = publicKey;
    info.mPrivateKey    = privateKey;

    // TODO: passphrase
    this->mKeyPair = info.makeKeyPair ();
}

//----------------------------------------------------------------//
Signature CryptoKeyPair::sign ( const Digest& digest, string hashAlgorithm ) const {

    Digest sig;

    int nid = Digest::nid ( hashAlgorithm );
    assert ( nid );

    if ( this->mKeyPair ) {
        switch ( this->mKeyPair->type ()) {

            case Poco::Crypto::KeyPair::KT_EC: {

                const Poco::Crypto::ECKey* pocoECKey = dynamic_cast < const Poco::Crypto::ECKey* >( this->mKeyPair.get ());
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
                
                const Poco::Crypto::RSAKey* pocoRSAKey = dynamic_cast < const Poco::Crypto::RSAKey* >( this->mKeyPair.get ());
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
Signature CryptoKeyPair::sign ( const Digest::DigestFunc& digestFunc, string hashAlgorithm ) const {
    
    Digest digest ( digestFunc, hashAlgorithm );
    return this->sign ( digest, hashAlgorithm );
}

//----------------------------------------------------------------//
Signature CryptoKeyPair::sign ( const AbstractSerializable& serializable, string hashAlgorithm ) const {

    return this->sign (
        [ & ]( Poco::DigestOutputStream& stream ) {
            ToJSONSerializer::toDigest ( serializable, stream );
        },
        hashAlgorithm
    );
}

//----------------------------------------------------------------//
Signature CryptoKeyPair::sign ( string message, string hashAlgorithm ) const {

    return this->sign (
        [ = ]( Poco::DigestOutputStream& stream ) {
            stream << message;
        },
        hashAlgorithm
    );
}

//----------------------------------------------------------------//
string CryptoKeyPair::toPEM () const {

    CryptoKeyInfo info ( this->mKeyPair.get (), CryptoKeyInfo::ENCODE_AS_PEM );
    return info.mPrivateKey.size () ? info.mPrivateKey : info.mPublicKey;
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
AbstractCryptoKey::KeyPairPtr CryptoKeyPair::AbstractCryptoKey_getKeyPair () const {

    return this->mKeyPair.get ();
}

//----------------------------------------------------------------//
void CryptoKeyPair::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    CryptoKeyInfo info;
    info.serializeFrom ( serializer );
    this->mKeyPair = info.makeKeyPair ();
}

//----------------------------------------------------------------//
void CryptoKeyPair::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    CryptoKeyInfo info ( this->mKeyPair.get ());
    info.serializeTo ( serializer );
}

} // namespace Volition
