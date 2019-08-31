// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_KEYDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_KEYDETAILSHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// KeyDetailsHandler
//================================================================//
class KeyDetailsHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        string keyID = this->getMatchString ( "keyHash" );
        
        ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
        const Ledger& ledger = scopedLock.getWebMiner ().getLedger ();

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
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
