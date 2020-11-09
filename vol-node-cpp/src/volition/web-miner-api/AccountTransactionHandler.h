// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_ACCOUNTTRANSACTIONSHANDLER_H
#define VOLITION_WEBMINERAPI_ACCOUNTTRANSACTIONSHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/MinerAPIFactory.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/Transaction.h>
#include <volition/TransactionStatus.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// AccountTransactionHandler
//================================================================//
class AccountTransactionHandler :
    public MinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET_PUT )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
    
        string accountName  = this->getMatchString ( "accountName" );
        string uuid         = this->getMatchString ( "uuid" );
    
        ScopedMinerLock scopedLock ( this->mWebMiner );
    
        switch ( method ) {
    
            case HTTP::GET: {

                TransactionStatus status = this->mWebMiner->getTransactionStatus ( accountName, uuid );
                
                jsonOut.set ( "status", status.getStatusCodeString ());
                jsonOut.set ( "message", status.mMessage );
                jsonOut.set ( "uuid", status.mUUID );
                return Poco::Net::HTTPResponse::HTTP_OK;
            }
            
            case HTTP::PUT: {

                SerializableUniquePtr < Transaction > transaction;
                FromJSONSerializer::fromJSON ( transaction, jsonIn );

                if ( transaction && transaction->checkMaker ( accountName, uuid )) {
                    this->mWebMiner->pushTransaction ( move ( transaction ));
                    jsonOut.set ( "status", "OK" );
                    return Poco::Net::HTTPResponse::HTTP_OK;
                }
                return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
            }
            
            default:
                break;
        }
        return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
