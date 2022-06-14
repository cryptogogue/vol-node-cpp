// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_IDENTIFYACCOUNT_H
#define VOLITION_TRANSACTIONS_IDENTIFYACCOUNT_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/ContractWithDigest.h>
#include <volition/Munge.h>
#include <volition/Policy.h>
#include <volition/Signature.h>

namespace Volition {
namespace Transactions {

//================================================================//
// Gamercert
//================================================================//
class Gamercert :
    public AbstractSerializable {
public:

    string      mKeyName;
    string      mSignature;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "keyName",           this->mKeyName );
        serializer.serialize ( "signature",         this->mSignature );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        UNUSED ( serializer );
    }
};

//================================================================//
// IdentifyAccount
//================================================================//
class IdentifyAccount :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "IDENTIFY_ACCOUNT" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    Gamercert       mGamercert;
    
    //----------------------------------------------------------------//
    string verify ( string ed25519PublicHex, string sigHex ) const {

        std::vector < unsigned char > pkeyBin ( crypto_sign_PUBLICKEYBYTES, 0 );
        sodium_hex2bin ( pkeyBin.data (), crypto_sign_PUBLICKEYBYTES, ed25519PublicHex.c_str (), ed25519PublicHex.size (), NULL, NULL, NULL );
        
        std::vector < unsigned char > sigBin ( sigHex.size () >> 1, 0 );
        sodium_hex2bin ( sigBin.data (), sigBin.size (), sigHex.c_str (), sigHex.size (), NULL, NULL, NULL );
        
        std::vector < unsigned char > msgBin (( sigBin.size () - crypto_sign_PUBLICKEYBYTES ) + 1, 0 );
        
        if ( crypto_sign_open ( msgBin.data (), NULL, sigBin.data (), sigBin.size (), pkeyBin.data ()) != 0 ) {
            return "";
        }

        return string (( char* )msgBin.data ());
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "gamercert",         this->mGamercert );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "gamercert",         this->mGamercert );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::IDENTIFY_ACCOUNT )) return "Permission denied.";
                
        AbstractLedger& ledger = context.mLedger;
        
        string identityKey = ledger.getIdentityKey ( this->mGamercert.mKeyName );
        if ( identityKey.size () == 0 ) return "Missing or inactive identity key.";

        string jsonBody = this->verify ( identityKey, this->mGamercert.mSignature );
        if ( identityKey.size () == 0 ) return "Could not verify identity.";
        
        Poco::JSON::Object::Ptr json = Poco::JSON::Parser ().parse ( jsonBody ).extract < Poco::JSON::Object::Ptr >();

        string community = json->optValue < string >( "community", "" );
        if ( community != Format::tolower ( ledger.getIdentity ())) return "Wrong community (network identity) name.";

        string publicKeyHex = json->optValue < string >( "payload", "" );
        const CryptoKeyInfo txMakerKeyInfo ( context.mKeyAndPolicy.mKey.getKeyPair ());
        if ( txMakerKeyInfo.mPublicKey != publicKeyHex ) return "Mismatched public key.";
        
        Poco::JSON::Object::Ptr identity = json->getObject ( "identity" );
        Poco::JSON::Object::Ptr claims = identity->getObject ( "claims" );
        
        bool pretend = claims->optValue < bool >( "pretend", true );
        if ( pretend ) return "Cannot use pretend identity.";
        
        string recourse = claims->getObject ( "recourse" )->optValue < string >( "type", "NONE" );
        if ( recourse != "FULL" ) return "Identity must be 'full recourse.'";

        string fingerprint = identity->optValue < string >( "aliasID", "" );
        if ( fingerprint.size () == 0 ) return "Missing identity fingerprint.";

        context.mAccountODBM.setIdentity ( fingerprint, this->mGamercert.mKeyName );
        
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
