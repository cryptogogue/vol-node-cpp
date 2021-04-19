// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_TRANSACTIONHANDLER_H
#define VOLITION_WEBMINERAPI_TRANSACTIONHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/Transaction.h>
#include <volition/TransactionStatus.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// TransactionHandler
//================================================================//
class TransactionHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET_PUT )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
    
        ScopedExclusiveMinerLock minerLock ( miner );
        AbstractLedger& ledger = miner->getLedger ();
    
        string accountName  = this->getMatchString ( "accountName" );
        string uuid         = this->getMatchString ( "uuid" );
    
        TransactionQueue& transactionQueue = miner->getTransactionQueue ();
    
        switch ( method ) {
    
            case HTTP::GET: {

                TransactionStatus status = transactionQueue.getTransactionStatus ( ledger, accountName, uuid );
                
                jsonOut.set ( "status", status.getStatusCodeString ());
                jsonOut.set ( "message", status.mMessage );
                jsonOut.set ( "uuid", status.mUUID );
                return Poco::Net::HTTPResponse::HTTP_OK;
            }
            
            case HTTP::PUT: {

                SerializableUniquePtr < Transaction > transaction;
                FromJSONSerializer::fromJSON ( transaction, jsonIn );

                if ( transaction && transaction->checkMaker ( accountName, uuid )) {
                    transactionQueue.pushTransaction ( move ( transaction ));
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
