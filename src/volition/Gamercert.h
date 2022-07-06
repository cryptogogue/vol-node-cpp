// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_GAMERCERT_H
#define VOLITION_GAMERCERT_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
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
    TransactionResult verify ( string identityKeyHex, string expectedCommunity, string expectedPayload, string& fingerprint ) const {
        
        LGN_LOG_SCOPE ( VOL_FILTER_JSON, INFO, "%s", __PRETTY_FUNCTION__ );
        
        if ( identityKeyHex.size () == 0 ) return "Missing or inactive identity key.";

        string jsonBody = this->verify ( identityKeyHex, this->mSignature );
        if ( jsonBody.size () == 0 ) return "Could not verify identity.";
        
        Poco::JSON::Object::Ptr json;
        
        try {
            json = Poco::JSON::Parser ().parse ( jsonBody ).extract < Poco::JSON::Object::Ptr >();
        }
        catch ( Poco::Exception& exception ) {
            LGN_LOG ( VOL_FILTER_JSON, ERROR, "%s", exception.displayText ().c_str ());
            return "Error parsing JSON.";
        }

        string community = json->optValue < string >( "community", "" );
        if ( community != Format::tolower ( expectedCommunity )) return "Wrong community (network identity) name.";

        string payload = json->optValue < string >( "payload", "" );
        if ( payload != expectedPayload ) return "Mismatched public key.";
        
        Poco::JSON::Object::Ptr identity = json->getObject ( "identity" );
        Poco::JSON::Object::Ptr claims = identity->getObject ( "claims" );
        
        bool pretend = claims->optValue < bool >( "pretend", true );
        if ( pretend ) return "Cannot use pretend identity.";
        
        string recourse = claims->getObject ( "recourse" )->optValue < string >( "type", "NONE" );
        if ( recourse != "FULL" ) return "Identity must be 'full recourse.'";

        fingerprint = identity->optValue < string >( "aliasID", "" );
        if ( fingerprint.size () == 0 ) return "Missing identity fingerprint.";
        
        return true;
    }
    
    //----------------------------------------------------------------//
    static string verify ( string ed25519PublicHex, string sigHex ) {

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
        
        serializer.serialize ( "keyName",           this->mKeyName );
        serializer.serialize ( "signature",         this->mSignature );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        UNUSED ( serializer );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
