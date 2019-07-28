// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_ACCOUNTDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_ACCOUNTDETAILSHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// AccountDetailsHandler
//================================================================//
class AccountDetailsHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        string accountName = this->getMatchString ( "accountName" );
        
        ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
        const Ledger& ledger = scopedLock.getWebMiner ().getLedger ();

        shared_ptr < Account > account = ledger.getAccount ( accountName );
        if ( account ) {
            Poco::JSON::Object::Ptr accountJSON = new Poco::JSON::Object ();
            accountJSON->set ( "accountName", accountName.c_str ());
            accountJSON->set ( "balance", ( int )account->getBalance ());
            accountJSON->set ( "nonce", ( int )account->getNonce ());

            jsonOut.set ( "account", accountJSON );
            return Poco::Net::HTTPResponse::HTTP_OK;
        }
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
