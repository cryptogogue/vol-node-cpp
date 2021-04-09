// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_KEYDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_KEYDETAILSHANDLER_H

#include <volition/AccountODBM.h>
#include <volition/Block.h>
#include <volition/SemiBlockingMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// KeyDetailsHandler
//================================================================//
class KeyDetailsHandler :
    public SemiBlockingMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus SemiBlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, AbstractLedger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        string keyID = this->getMatchString ( "keyHash" );

        shared_ptr < AccountKeyLookup > accountKeyLookup = ledger.getAccountKeyLookup ( keyID );
        if ( accountKeyLookup ) {
            
            string accountName = AccountODBM ( ledger, accountKeyLookup->mAccountIndex ).mName.get ();
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
