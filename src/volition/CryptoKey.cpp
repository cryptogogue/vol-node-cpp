// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/CryptoKey.h>
#include <volition/FNV1a.h>

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

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void CryptoKey::AbstractSerializable_serialize ( AbstractSerializer& serializer ) {

    if ( serializer.getMode () == AbstractSerializer::SERIALIZE_IN ) {
    
        string type = serializer.serializeIn < string >( "type", "" );
        
        switch ( FNV1a::hash_64 ( type.c_str ())) {
            case FNV1a::const_hash_64 ( "EC_HEX" ): {
                
                // load the public key
                
                string groupName = serializer.serializeIn < string >( "groupName", "" );
                EC_GROUP* ecGroup = EC_GROUP_new_by_curve_name ( CryptoKey::getNIDFromGroupName ( groupName ));
                assert ( ecGroup );
                
                string ecPubKeyHexStr = serializer.serializeIn < string >( "publicKeyHex", ecPubKeyHexStr );
                assert ( ecPubKeyHexStr.size ());
                
                EC_POINT* ecPoint = EC_POINT_hex2point ( ecGroup, ecPubKeyHexStr.c_str (), NULL, NULL );
                assert ( ecPoint );
                
                EC_KEY* ecKey = EC_KEY_new ();
                EC_KEY_set_public_key ( ecKey, ecPoint );
                
                EC_GROUP_free ( ecGroup );
                EC_POINT_free ( ecPoint );
                
                // load the private key
                
                string ecPrivKeyHexStr = serializer.serializeIn < string >( "privateKeyHex", ecPrivKeyHexStr );
                
                if ( ecPrivKeyHexStr.size ()) {
                    
                    BIGNUM* ecPrivKey = NULL;
                    BN_hex2bn( &ecPrivKey, ecPrivKeyHexStr.c_str ());
                    assert ( ecPrivKey );
                    
                    EC_KEY_set_private_key ( ecKey, ecPrivKey );
                    BN_free ( ecPrivKey );
                }
                
                // convert to Poco key
                
                EVP_PKEY* pkey = EVP_PKEY_new ();
                EVP_PKEY_set1_EC_KEY ( pkey, ecKey );

                this->mKeyPair = make_shared < Poco::Crypto::ECKey >( Poco::Crypto::EVPPKey { pkey }); // prevent the 'most vexing parse'
                
                EC_KEY_free ( ecKey );
                EVP_PKEY_free ( pkey );
                
                break;
            }
            case FNV1a::const_hash_64 ( "EC_PEM" ): {
            
                string pem = serializer.serializeIn < string >( "pem", "" );
                stringstream keyStream ( pem );
                this->mKeyPair = pem.size () ? make_shared < Poco::Crypto::ECKey >( &keyStream ) : NULL;
                
                break;
            }
            case FNV1a::const_hash_64 ( "RSA_PEM" ): {
                break;
            }
        }
    }
    else if ( this->mKeyPair ) {
        
        switch ( this->mKeyPair->type ()) {
            
            case Poco::Crypto::KeyPair::KT_EC: {
            
                Poco::Crypto::ECKey* pocoECKey = dynamic_cast < Poco::Crypto::ECKey* >( this->mKeyPair.get ());
                assert ( pocoECKey &&  pocoECKey->impl ());
                
                EC_KEY* ecKey = pocoECKey->impl ()->getECKey ();
                
                serializer.serializeOut < string >( "type", "EC_HEX" );
                
                const EC_GROUP* ecGroup = EC_KEY_get0_group ( ecKey );
                string groupName = CryptoKey::getGroupNameFromNID ( EC_GROUP_get_curve_name ( ecGroup ));
                serializer.serializeOut < string >( "groupName", groupName );
                
                const EC_POINT* ecPubKey = EC_KEY_get0_public_key ( ecKey );
                assert ( ecPubKey );
                
                char* ecPubKeyHex = EC_POINT_point2hex ( ecGroup , ecPubKey, POINT_CONVERSION_COMPRESSED, NULL );
                string ecPubKeyHexStr = ecPubKeyHex;
                OPENSSL_free ( ecPubKeyHex );
                
                serializer.serializeOut < string >( "publicKeyHex", ecPubKeyHexStr );
                
                const BIGNUM* ecPrivKey = EC_KEY_get0_private_key ( ecKey );
                if ( ecPrivKey ) {
                    char* ecPrivKeyHex = BN_bn2hex ( ecPrivKey );
                    string ecPrivKeyHexStr = ecPrivKeyHex;
                    OPENSSL_free ( ecPrivKeyHex );
                    
                    serializer.serializeOut < string >( "privateKeyHex", ecPrivKeyHexStr );
                }
                
                break;
            }
             case Poco::Crypto::KeyPair::KT_RSA: {
                
                serializer.serializeOut < string >( "type", "RSA_PEM" );
                break;
            }
        }
    }
}

} // namespace Volition
