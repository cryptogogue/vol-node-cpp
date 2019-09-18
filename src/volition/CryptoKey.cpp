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

    CryptoKeyInfo info ( *this, CryptoKeyInfo::HEX );
    if ( !info ) return "";
    
    Poco::Crypto::DigestEngine digestEngine ( "SHA256" );
    Poco::DigestOutputStream digestStream ( digestEngine );
    digestStream << Format::toupper ( info.mPublicKey );
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
