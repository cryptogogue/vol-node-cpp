// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_KEYDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_KEYDETAILSHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/WebMinerAPIFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// KeyDetailsHandler
//================================================================//
class KeyDetailsHandler :
    public WebMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        string keyID = this->getMatchString ( "keyHash" );
        
        ScopedWebMinerLock scopedLock ( this->mWebMiner );
        const Ledger& ledger = this->mWebMiner->getLedger ();

        shared_ptr < AccountKeyLookup > accountKeyLookup = ledger.getAccountKeyLookup ( keyID );
        if ( accountKeyLookup ) {
            
            string accountName = ledger.getAccountName ( accountKeyLookup->mAccountIndex );
            if ( accountName.size () > 0 ) {
            
                Poco::JSON::Object::Ptr accountLookupJSON = new Poco::JSON::Object ();
                accountLookupJSON->set ( "accountName", accountName.c_str ());
                accountLookupJSON->set ( "keyName", accountKeyLookup->mKeyName.c_str ());
                
                jsonOut.set ( "keyInfo", accountLookupJSON );
                return Poco::Net::HTTPResponse::HTTP_OK;
            }
        }
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
