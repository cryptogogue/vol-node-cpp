// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/CryptoPublicKey.h>
#include <volition/CryptoKeyInfo.h>
#include <volition/FNV1a.h>
#include <volition/Format.h>

#include <openssl/ecdsa.h>
#include <openssl/rsa.h>

namespace Volition {

//================================================================//
// CryptoPublicKey
//================================================================//

//----------------------------------------------------------------//
CryptoPublicKey::CryptoPublicKey () {
}

//----------------------------------------------------------------//
CryptoPublicKey::CryptoPublicKey ( CryptoKeyInfo keyInfo ) {

    keyInfo.mPrivateKey.clear ();
    this->mKeyPair = keyInfo.makeKeyPair ();
}

//----------------------------------------------------------------//
CryptoPublicKey::~CryptoPublicKey () {
}

//----------------------------------------------------------------//
void CryptoPublicKey::rsaFromPEM ( string publicKey ) {
    
    CryptoKeyInfo info;
    info.mFormat        = CryptoKeyInfo::RSA_PEM;
    info.mPublicKey     = publicKey;

    // TODO: passphrase
    this->mKeyPair = info.makeKeyPair ();
}

//----------------------------------------------------------------//
string CryptoPublicKey::toPEM () const {

    CryptoKeyInfo info ( this->mKeyPair.get (), CryptoKeyInfo::ENCODE_AS_PEM );
    return info.mPublicKey;
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
AbstractCryptoKey::KeyPairPtr CryptoPublicKey::AbstractCryptoKey_getKeyPair () const {

    return this->mKeyPair.get ();
}

//----------------------------------------------------------------//
void CryptoPublicKey::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    CryptoKeyInfo info;
    info.serializeFrom ( serializer );
    info.mPrivateKey.clear ();
    this->mKeyPair = info.makeKeyPair ();
}

//----------------------------------------------------------------//
void CryptoPublicKey::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    CryptoKeyInfo info ( this->mKeyPair.get ());
    info.mPrivateKey.clear ();
    info.serializeTo ( serializer );
}

} // namespace Volition
