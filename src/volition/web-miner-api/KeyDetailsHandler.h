// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_KEYDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_KEYDETAILSHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionFactory.h>
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
    
        string keyID = this->getMatchString ( "keyHash" );
        
        ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
        const Ledger& ledger = scopedLock.getWebMiner ().getLedger ();

        shared_ptr < KeyInfo > keyInfo = ledger.getKeyInfo ( keyID );
        if ( keyInfo ) {
            Poco::JSON::Object::Ptr keyInfoJSON = new Poco::JSON::Object ();
            keyInfoJSON->set ( "accountName", keyInfo->mAccountName.c_str ());
            keyInfoJSON->set ( "keyName", keyInfo->mKeyName.c_str ());
            
            jsonOut.set ( "keyInfo", keyInfoJSON );
            return Poco::Net::HTTPResponse::HTTP_OK;
        }
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
