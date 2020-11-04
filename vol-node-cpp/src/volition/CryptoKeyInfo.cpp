// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/CryptoKeyInfo.h>
#include <volition/FNV1a.h>
#include <volition/Format.h>

#include <openssl/ecdsa.h>
#include <openssl/rsa.h>

namespace Volition {

//================================================================//
// CryptoKeyInfo
//================================================================//

//----------------------------------------------------------------//
CryptoKeyInfo::CryptoKeyInfo () :
    mFormat ( UNKNOWN ) {
}

//----------------------------------------------------------------//
CryptoKeyInfo::CryptoKeyInfo ( const Poco::Crypto::KeyPair* keyPair, EncodeAs encodeAs ) :
    mFormat ( UNKNOWN ) {
    
    if ( keyPair ) {
        switch ( keyPair->type ()) {
            
            case Poco::Crypto::KeyPair::KT_EC:
                this->initFromEC ( keyPair, encodeAs );
                break;

            case Poco::Crypto::KeyPair::KT_RSA:
                this->initFromRSA ( keyPair, encodeAs );
                break;

            default:
                break;
        }
    }
}

//----------------------------------------------------------------//
void CryptoKeyInfo::dumpRSAHex ( const Poco::Crypto::KeyPair* keyPair ) {

    const Poco::Crypto::RSAKey* pocoRSAKey = dynamic_cast < const Poco::Crypto::RSAKey* >( keyPair );
    assert ( pocoRSAKey );

    RSA* rsaKey = pocoRSAKey->impl ()->getRSA ();
    assert ( rsaKey );
    
    const BIGNUM* n; // - modulus
    const BIGNUM* e; // - public exponent
    const BIGNUM* d; // - private exponent
    
    // PKCS 3.2.1 - get the modulus and exponents
    RSA_get0_key ( rsaKey, &n, &e, &d );
    
    const BIGNUM* p; // - first factor
    const BIGNUM* q; // - second factor
    const BIGNUM* dmp1; // - first factor's CRT exponent
    const BIGNUM* dmq1; // - second factor's CRT exponent
    const BIGNUM* iqmp; // - first CRT coefficient
    
    // PKCS 3.2.2 - get the private key
    RSA_get0_factors ( rsaKey, &p, &q );
    RSA_get0_crt_params ( rsaKey, &dmp1, &dmq1, &iqmp );
    
    printf ( "%s\n", CryptoKeyInfo::openSSLString ( BN_bn2hex ( n )).c_str ());
    printf ( "%s\n", CryptoKeyInfo::openSSLString ( BN_bn2hex ( e )).c_str ());
    printf ( "%s\n", CryptoKeyInfo::openSSLString ( BN_bn2hex ( d )).c_str ());
    printf ( "%s\n", CryptoKeyInfo::openSSLString ( BN_bn2hex ( p )).c_str ());
    printf ( "%s\n", CryptoKeyInfo::openSSLString ( BN_bn2hex ( q )).c_str ());
    printf ( "%s\n", CryptoKeyInfo::openSSLString ( BN_bn2hex ( dmp1 )).c_str ());
    printf ( "%s\n", CryptoKeyInfo::openSSLString ( BN_bn2hex ( dmq1 )).c_str ());
    printf ( "%s\n", CryptoKeyInfo::openSSLString ( BN_bn2hex ( iqmp )).c_str ());
}

//----------------------------------------------------------------//
CryptoKeyInfo::Format CryptoKeyInfo::getFormatFromString ( string format ) {

    switch ( FNV1a::hash_64 ( format.c_str ())) {
        case FNV1a::const_hash_64 ( "EC_HEX" ):     return EC_HEX;
        case FNV1a::const_hash_64 ( "EC_PEM" ):     return EC_PEM;
        case FNV1a::const_hash_64 ( "RSA_HEX" ):    return RSA_HEX;
        case FNV1a::const_hash_64 ( "RSA_PEM" ):    return RSA_PEM;
    }
    return UNKNOWN;
}

//----------------------------------------------------------------//
string CryptoKeyInfo::getKeyID ( const Poco::Crypto::KeyPair* keyPair ) {

    assert ( keyPair );
    
    switch ( keyPair->type ()) {
        
        case Poco::Crypto::KeyPair::KT_EC: {
            
            const Poco::Crypto::ECKey* pocoECKey = dynamic_cast < const Poco::Crypto::ECKey* >( keyPair );
            assert ( pocoECKey );

            EC_KEY* ecKey = pocoECKey->impl ()->getECKey ();
            assert ( ecKey );
            
            const EC_GROUP* ecGroup     = EC_KEY_get0_group ( ecKey );
            const EC_POINT* ecPubKey    = EC_KEY_get0_public_key ( ecKey );
            
            return Digest ( CryptoKeyInfo::openSSLString ( EC_POINT_point2hex ( ecGroup , ecPubKey, POINT_CONVERSION_COMPRESSED, NULL )), Digest::HASH_ALGORITHM_SHA256 );

        }
        case Poco::Crypto::KeyPair::KT_RSA: {
        
            const Poco::Crypto::RSAKey* pocoRSAKey = dynamic_cast < const Poco::Crypto::RSAKey* >( keyPair );
            assert ( pocoRSAKey );

            RSA* rsaKey = pocoRSAKey->impl ()->getRSA ();
            assert ( rsaKey );
            
            const BIGNUM* n; // - modulus
            const BIGNUM* e; // - public exponent
            const BIGNUM* d; // - private exponent
            
            // PKCS 3.2.1 - get the modulus and exponents
            RSA_get0_key ( rsaKey, &n, &e, &d );
            
            string ident = Volition::Format::write (
                "%s:%s",
                CryptoKeyInfo::openSSLString ( BN_bn2hex ( n )).c_str (),
                CryptoKeyInfo::openSSLString ( BN_bn2hex ( e )).c_str ()
            );
            
            return Digest ( ident, Digest::HASH_ALGORITHM_SHA256 );
        }
    }
    assert ( false );
    return "";
}

//----------------------------------------------------------------//
string CryptoKeyInfo::getStringFromFormat ( Format format ) {

    switch ( format ) {
        case EC_HEX:        return "EC_HEX";
        case EC_PEM:        return "EC_PEM";
        case RSA_HEX:       return "RSA_HEX";
        case RSA_PEM:       return "RSA_PEM";
        case UNKNOWN:       return "UNKNOWN";
    }
    return "";
}

//----------------------------------------------------------------//
void CryptoKeyInfo::initAsPEM ( const Poco::Crypto::KeyPair* keyPair ) {

    if ( !keyPair ) return;

    stringstream pubKeyStream;
    stringstream privKeyStream;

    keyPair->save ( &pubKeyStream, &privKeyStream );

    this->mPublicKey    = pubKeyStream.str ();
    this->mPrivateKey   = privKeyStream.str ();
    
    // TODO: password
}

//----------------------------------------------------------------//
void CryptoKeyInfo::initFromEC ( const Poco::Crypto::KeyPair* keyPair, EncodeAs encodeAs ) {

    if ( !keyPair ) return;
    assert ( keyPair->type () == Poco::Crypto::KeyPair::KT_EC );

    switch ( encodeAs ) {

        case ENCODE_AS_ANY:
        case ENCODE_AS_HEX: {

            const Poco::Crypto::ECKey* pocoECKey = dynamic_cast < const Poco::Crypto::ECKey* >( keyPair );
            assert ( pocoECKey );

            EC_KEY* ecKey = pocoECKey->impl ()->getECKey ();
            assert ( ecKey );
            
            const EC_GROUP* ecGroup = EC_KEY_get0_group ( ecKey );
            this->mGroupName = Poco::Crypto::ECKey::getCurveName ( EC_GROUP_get_curve_name ( ecGroup ));
            
            const EC_POINT* ecPubKey = EC_KEY_get0_public_key ( ecKey );
            assert ( ecPubKey );
            
            this->mPublicKey  = CryptoKeyInfo::openSSLString ( EC_POINT_point2hex ( ecGroup , ecPubKey, POINT_CONVERSION_COMPRESSED, NULL ));
            
            const BIGNUM* ecPrivKey = EC_KEY_get0_private_key ( ecKey );
            if ( ecPrivKey ) {
                this->mPrivateKey = CryptoKeyInfo::openSSLString ( BN_bn2hex ( ecPrivKey ));
            }
            
            this->mFormat = ( Format )( TYPE_EC | ENCODING_HEX );
            break;
        }
        
        case ENCODE_AS_PEM: {
                    
            this->initAsPEM ( keyPair );
            this->mFormat = ( Format )( TYPE_EC | ENCODING_PEM );
            break;
        }
        
        default:
            assert ( false );
            break;
    }
}

//----------------------------------------------------------------//
void CryptoKeyInfo::initFromRSA ( const Poco::Crypto::KeyPair* keyPair, EncodeAs encodeAs ) {

    if ( !keyPair ) return;
    assert ( keyPair->type () == Poco::Crypto::KeyPair::KT_RSA );

    switch ( encodeAs ) {

        case ENCODE_AS_HEX: {

            assert ( false );
            break;
        }
        
        case ENCODE_AS_ANY:
        case ENCODE_AS_PEM: {
            
            this->initAsPEM ( keyPair );
            this->mFormat = ( Format )( TYPE_RSA | ENCODING_PEM );
            break;
        }
        
        default:
            assert ( false );
            break;
    }
}

//----------------------------------------------------------------//
CryptoKeyInfo::KeyPairPtr CryptoKeyInfo::makeKeyPair () const {

    switch ( this->mFormat & TYPE_MASK ) {
        case TYPE_EC:    return this->makeKeyPairEC ();
        case TYPE_RSA:   return this->makeKeyPairRSA ();
    }
    return NULL;
}

//----------------------------------------------------------------//
CryptoKeyInfo::KeyPairPtr CryptoKeyInfo::makeKeyPairEC () const {

    assert ( this->mFormat & TYPE_EC );

    CryptoKeyInfo::KeyPairPtr keyPair;

    switch ( this->mFormat & ENCODING_MASK ) {
    
        case ENCODING_HEX: {
            
            // load the public key
            string groupName = this->mGroupName;
            assert ( groupName.size ());
            EC_GROUP* ecGroup = EC_GROUP_new_by_curve_name ( Poco::Crypto::ECKey::getCurveNID ( groupName ));
            assert ( ecGroup );
            
            string ecPubKeyHexStr = this->mPublicKey;
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
            string ecPrivKeyHexStr = this->mPrivateKey;
            
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

            keyPair = make_shared < Poco::Crypto::ECKey >( Poco::Crypto::EVPPKey { pkey }); // prevent the 'most vexing parse'

            EVP_PKEY_free ( pkey );
            
            break;
        }
        case ENCODING_PEM: {
            
            stringstream publicKeyStream ( this->mPublicKey );
            stringstream privateKeyStream ( this->mPrivateKey );

            keyPair = make_shared < Poco::Crypto::ECKey >(
                this->mPublicKey.size () ? &publicKeyStream : NULL,
                this->mPrivateKey.size () ? &privateKeyStream : NULL
            ); // TODO: password
            
            break;
        }
    }
    return keyPair;
}

//----------------------------------------------------------------//
CryptoKeyInfo::KeyPairPtr CryptoKeyInfo::makeKeyPairRSA () const {

    assert ( this->mFormat & TYPE_RSA );

    CryptoKeyInfo::KeyPairPtr keyPair;

    switch ( this->mFormat & ENCODING_MASK ) {
        case ENCODING_HEX: {
            
            assert ( false );
            break;
        }
        case ENCODING_PEM: {

            stringstream publicKeyStream ( this->mPublicKey );
            stringstream privateKeyStream ( this->mPrivateKey );

            keyPair = make_shared < Poco::Crypto::RSAKey >(
                this->mPublicKey.size () ? &publicKeyStream : NULL,
                this->mPrivateKey.size () ? &privateKeyStream : NULL
            ); // TODO: password
            
            break;
        }
    }
    return keyPair;
}

//----------------------------------------------------------------//
string CryptoKeyInfo::openSSLString ( char* c ) {

    string s = c;
    OPENSSL_free ( c );
    return s;
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
void CryptoKeyInfo::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
        
    string type = serializer.serializeIn < string >( "type", "" );
    this->mFormat = CryptoKeyInfo::getFormatFromString ( type );
        
    serializer.serialize ( "groupName", this->mGroupName );
    serializer.serialize ( "publicKey", this->mPublicKey );
    serializer.serialize ( "privateKey", this->mPrivateKey );
}

//----------------------------------------------------------------//
void CryptoKeyInfo::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    if ( this->mFormat == UNKNOWN ) return;
        
    serializer.serialize < string >( "type", CryptoKeyInfo::getStringFromFormat ( this->mFormat ));
    serializer.serialize < string >( "publicKey", this->mPublicKey );
    
    if ( this->mPrivateKey.size ()) {
        serializer.serialize < string >( "privateKey", this->mPrivateKey );
    }
    
    if ( this->mFormat == EC_HEX ) {
        serializer.serialize < string >( "groupName", this->mGroupName );
    }
}

} // namespace Volition
