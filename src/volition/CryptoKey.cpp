// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/CryptoKey.h>
#include <volition/FNV1a.h>
#include <volition/Format.h>

#include <openssl/ecdsa.h>

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
CryptoKeyInfo CryptoKey::getInfo () const {

    CryptoKeyInfo info;
    info.mIsValid = false;
    
    if ( !this->mKeyPair ) return info;
    
    switch ( this->mKeyPair->type ()) {
        
        case Poco::Crypto::KeyPair::KT_EC: {
        
            const Poco::Crypto::ECKey& pocoECKey = *this;
            assert ( pocoECKey.impl ());
            
            EC_KEY* ecKey = pocoECKey.impl ()->getECKey ();
            
            info.mType = "EC_HEX";
            
            const EC_GROUP* ecGroup = EC_KEY_get0_group ( ecKey );
            info.mGroupName = CryptoKey::getGroupNameFromNID ( EC_GROUP_get_curve_name ( ecGroup ));
            
            const EC_POINT* ecPubKey = EC_KEY_get0_public_key ( ecKey );
            assert ( ecPubKey );
            
            char* ecPubKeyHex = EC_POINT_point2hex ( ecGroup , ecPubKey, POINT_CONVERSION_COMPRESSED, NULL );
            info.mPubKeyHex  = ecPubKeyHex;
            OPENSSL_free ( ecPubKeyHex );
            
            const BIGNUM* ecPrivKey = EC_KEY_get0_private_key ( ecKey );
            if ( ecPrivKey ) {
                char* ecPrivKeyHex = BN_bn2hex ( ecPrivKey );
                info.mPrivKeyHex = ecPrivKeyHex;
                OPENSSL_free ( ecPrivKeyHex );
            }
            info.mIsValid = true;
            break;
        }
        case Poco::Crypto::KeyPair::KT_RSA: {
            // TODO: RSA
            info.mType = "RSA_PEM";
            break;
        }
    }
    return info;
}



//----------------------------------------------------------------//
string CryptoKey::getKeyID () const {

    CryptoKeyInfo info = this->getInfo ();
    if ( !info.mIsValid ) return "";
    
    Poco::Crypto::DigestEngine digestEngine ( "SHA256" );
    Poco::DigestOutputStream digestStream ( digestEngine );
    digestStream << Format::toupper ( info.mPubKeyHex );
    digestStream.close ();
    
    return Poco::Crypto::DigestEngine::digestToHex ( digestEngine.digest ());
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
Signature CryptoKey::sign ( const DigestFunc& digestFunc, string hashAlgorithm ) const {

    if ( this->mKeyPair ) {
        switch ( this->mKeyPair->type ()) {
                
            case Poco::Crypto::KeyPair::KT_EC: {
            
                const Poco::Crypto::ECKey* pocoECKey = *this;
                assert ( pocoECKey );
                
                EC_KEY* pKey = pocoECKey->impl ()->getECKey ();
                assert ( pKey );
            
                Poco::Crypto::DigestEngine digestEngine ( hashAlgorithm );
                Poco::DigestOutputStream signatureStream ( digestEngine );
                digestFunc ( signatureStream );
                signatureStream.close ();
                
                Digest digest = digestEngine.digest ();
            
                uint sigLen = ( unsigned int )ECDSA_size ( pKey );
            
                Digest sig;
                sig.resize ( sigLen );
            
                int result = ECDSA_sign ( 0,
                    &digest [ 0 ], ( uint )digest.size (),
                    &sig [ 0 ], &sigLen,
                    pKey
                );
                assert ( result == 1 );
                
                if ( sigLen < sig.size ()) sig.resize ( sigLen );
                return Signature ( digest, sig, hashAlgorithm );
            }
            
            case Poco::Crypto::KeyPair::KT_RSA: {
                // TODO: RSA
            }
        }
    }
    return Signature ();
}

//----------------------------------------------------------------//
Signature CryptoKey::sign ( const AbstractSerializable& serializable, string hashAlgorithm ) const {

    return this->sign (
        [ & ]( Poco::DigestOutputStream& stream ) {
            SortedDigestSerializer::hash ( serializable, stream );
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
bool CryptoKey::verify ( const Signature& signature, const DigestFunc& digestFunc ) const {

    bool result = false;

    if ( this->mKeyPair && signature ) {
        switch ( this->mKeyPair->type ()) {
                
            case Poco::Crypto::KeyPair::KT_EC: {
            
                const Poco::Crypto::ECKey* pocoECKey = *this;
                assert ( pocoECKey );
                
                EC_KEY* pKey = pocoECKey->impl ()->getECKey ();
                assert ( pKey );
                
                Poco::Crypto::DigestEngine digestEngine ( signature.getHashAlgorithm ());
                Poco::DigestOutputStream signatureStream ( digestEngine );
                digestFunc ( signatureStream );
                signatureStream.close ();
                    
                Digest digest = digestEngine.digest ();
                Digest sig = signature.getSignature ();
            
                result = 1 == ECDSA_verify ( 0,
                    &digest [ 0 ], ( uint )digest.size (),
                    &sig [ 0 ], ( uint )sig.size (),
                    pKey
                );
                break;
            }
            
            case Poco::Crypto::KeyPair::KT_RSA: {
                // TODO: RSA
                break;
            }
        }
    }
    return result;
}

//----------------------------------------------------------------//
bool CryptoKey::verify ( const Signature& signature, const AbstractSerializable& serializable ) const {

    return this->verify ( signature, [ & ]( Poco::DigestOutputStream& stream ) {
        SortedDigestSerializer::hash ( serializable, stream );
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

    this->mKeyPair = NULL;
    
    string type = serializer.serializeIn < string >( "type", "" );

    switch ( FNV1a::hash_64 ( type.c_str ())) {
        case FNV1a::const_hash_64 ( "EC_HEX" ): {
                        
            // load the public key
            string groupName = serializer.serializeIn < string >( "groupName", "" );
            assert ( groupName.size ());
            EC_GROUP* ecGroup = EC_GROUP_new_by_curve_name ( CryptoKey::getNIDFromGroupName ( groupName ));
            assert ( ecGroup );
            
            string ecPubKeyHexStr = serializer.serializeIn < string >( "publicKey", "" );
            assert ( ecPubKeyHexStr.size ());
            EC_POINT* ecPoint = EC_POINT_hex2point ( ecGroup, ecPubKeyHexStr.c_str (), NULL, NULL );
            assert ( ecPoint );
            
            EC_KEY* ecKey = EC_KEY_new ();
            assert ( ecKey );
            EC_KEY_set_group ( ecKey, ecGroup );
            EC_KEY_set_public_key ( ecKey, ecPoint );
            
            EC_GROUP_free ( ecGroup );
            EC_POINT_free ( ecPoint );
            
            // load the private key
            string ecPrivKeyHexStr = serializer.serializeIn < string >( "privateKey", "" );
            
            if ( ecPrivKeyHexStr.size ()) {
               
                BIGNUM* ecPrivKey = NULL;
                BN_hex2bn ( &ecPrivKey, ecPrivKeyHexStr.c_str ());
                assert ( ecPrivKey );
                
                EC_KEY_set_private_key ( ecKey, ecPrivKey );
                BN_free ( ecPrivKey );
            }
            
            // convert to Poco key
            EVP_PKEY* pkey = EVP_PKEY_new ();
            assert ( pkey );
            
            EVP_PKEY_set1_EC_KEY ( pkey, ecKey );
            EC_KEY_free ( ecKey );

            this->mKeyPair = make_shared < Poco::Crypto::ECKey >( Poco::Crypto::EVPPKey { pkey }); // prevent the 'most vexing parse'

            EVP_PKEY_free ( pkey );
            
            break;
        }
        case FNV1a::const_hash_64 ( "EC_PEM" ): {
            
            string publicKey = serializer.serializeIn < string >( "publicKey", "" );
            string privateKey = serializer.serializeIn < string >( "privateKey", "" );

            if ( publicKey.size () || privateKey.size ()) {
            
                stringstream publicKeyStream ( publicKey );
                stringstream privateKeyStream ( privateKey );
            
                this->mKeyPair = make_shared < Poco::Crypto::ECKey >(
                    publicKey.size () ? &publicKeyStream : NULL,
                    privateKey.size () ? &privateKeyStream : NULL
                );
            }
            break;
        }
        case FNV1a::const_hash_64 ( "RSA_PEM" ): {
            // TODO: RSA
            break;
        }
    }
}

//----------------------------------------------------------------//
void CryptoKey::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    CryptoKeyInfo info = this->getInfo ();
    if ( !info.mIsValid ) return;
    
    serializer.serialize < string >( "type", info.mType );
    
    switch ( this->mKeyPair->type ()) {
        
        case Poco::Crypto::KeyPair::KT_EC: {
            
            serializer.serialize < string >( "groupName", info.mGroupName );
            serializer.serialize < string >( "publicKey", info.mPubKeyHex );
            
            if ( info.mPrivKeyHex.size ()) {
                serializer.serialize < string >( "privateKey", info.mPrivKeyHex );
            }
            
            break;
        }
        case Poco::Crypto::KeyPair::KT_RSA: {
            // TODO: RSA
            break;
        }
    }
}

} // namespace Volition
