// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/CryptoKey.h>
#include <volition/CryptoKeyInfo.h>
#include <volition/FNV1a.h>
#include <volition/Format.h>

#include <openssl/ecdsa.h>

// bitaddress.org
// private: KzvyiCGoxXMaraHf3HmtZsttox2U99VTPq9RTWyJqsTVdEZ9nYDV
// public compressed: 0322FDB3D240158274B8FCA64263B04215A55B6F248AEB51DDED78F052C2A73DC1
// public: 0422FDB3D240158274B8FCA64263B04215A55B6F248AEB51DDED78F052C2A73DC135CB34215CFC80ECCAF0FE74982DDBA6A85F629C71D0BEB4434E4F7DEA1E623D

namespace Volition {

//================================================================//
// CryptoKeyInfo
//================================================================//

//----------------------------------------------------------------//
CryptoKeyInfo::CryptoKeyInfo () :
    mFormat ( UNKNOWN ) {
}

//----------------------------------------------------------------//
CryptoKeyInfo::CryptoKeyInfo ( const CryptoKey& cryptoKey, Encoding encoding ) :
    mFormat ( UNKNOWN ) {
        
    const Poco::Crypto::KeyPair* keyPair = cryptoKey;
    if ( !keyPair ) return;
    
    switch ( keyPair->type ()) {
        
        case Poco::Crypto::KeyPair::KT_EC: {
            this->initFromEC ( cryptoKey, encoding );
            break;
        }
        case Poco::Crypto::KeyPair::KT_RSA: {
            this->initFromRSA ( cryptoKey, encoding );
            break;
        }
    }
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
void CryptoKeyInfo::initFromEC ( const CryptoKey& cryptoKey, Encoding encoding ) {

    const Poco::Crypto::KeyPair* keyPair = cryptoKey;
    if ( !keyPair ) return;
    assert ( keyPair->type () == Poco::Crypto::KeyPair::KT_EC );

    switch ( encoding ) {

        case HEX: {

            const Poco::Crypto::ECKey* pocoECKey = dynamic_cast < const Poco::Crypto::ECKey* >( keyPair );
            assert ( pocoECKey && pocoECKey->impl ());
            
            EC_KEY* ecKey = pocoECKey->impl ()->getECKey ();
            
            const EC_GROUP* ecGroup = EC_KEY_get0_group ( ecKey );
            this->mGroupName = CryptoKey::getGroupNameFromNID ( EC_GROUP_get_curve_name ( ecGroup ));
            
            const EC_POINT* ecPubKey = EC_KEY_get0_public_key ( ecKey );
            assert ( ecPubKey );
            
            char* ecPubKeyHex = EC_POINT_point2hex ( ecGroup , ecPubKey, POINT_CONVERSION_COMPRESSED, NULL );
            this->mPublicKey  = ecPubKeyHex;
            OPENSSL_free ( ecPubKeyHex );
            
            const BIGNUM* ecPrivKey = EC_KEY_get0_private_key ( ecKey );
            if ( ecPrivKey ) {
                char* ecPrivKeyHex = BN_bn2hex ( ecPrivKey );
                this->mPrivateKey = ecPrivKeyHex;
                OPENSSL_free ( ecPrivKeyHex );
            }
            break;
        }
        
        case PEM: {
        
            stringstream pubKeyStream;
            stringstream privKeyStream;

            keyPair->save ( &pubKeyStream, &privKeyStream );

            this->mPublicKey = pubKeyStream.str ();
            this->mPrivateKey = privKeyStream.str ();
        
            break;
        }
        
        default:
            assert ( false );
            break;
    }
    
    this->mFormat = ( Format )( EC | encoding );
}

//----------------------------------------------------------------//
void CryptoKeyInfo::initFromRSA ( const CryptoKey& cryptoKey, Encoding encoding ) {
    UNUSED ( cryptoKey );
    UNUSED ( encoding );
    
    // TODO: RSA
}

//----------------------------------------------------------------//
CryptoKeyInfo::KeyPairPtr CryptoKeyInfo::makeKeyPair () const {

    switch ( this->mFormat & TYPE_MASK ) {
        case EC:    return this->makeKeyPairEC ();
        case RSA:   return this->makeKeyPairRSA ();
    }
    return NULL;
}

//----------------------------------------------------------------//
CryptoKeyInfo::KeyPairPtr CryptoKeyInfo::makeKeyPairEC () const {

    assert ( this->mFormat & EC );

    CryptoKeyInfo::KeyPairPtr keyPair;

    switch ( this->mFormat & ENCODING_MASK ) {
        case HEX: {
            
            // load the public key
            string groupName = this->mGroupName;
            assert ( groupName.size ());
            EC_GROUP* ecGroup = EC_GROUP_new_by_curve_name ( CryptoKey::getNIDFromGroupName ( groupName ));
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
        case PEM: {
            
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

    assert ( this->mFormat & RSA );

    // TODO: RSA
    return NULL;
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