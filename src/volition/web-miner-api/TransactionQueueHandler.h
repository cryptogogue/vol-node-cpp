// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_TRANSACTIONQUEUEHANDLER_H
#define VOLITION_WEBMINERAPI_TRANSACTIONQUEUEHANDLER_H

#include <volition/Block.h>
#include <volition/BlockingMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/Transaction.h>
#include <volition/TransactionStatus.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// TransactionQueueHandler
//================================================================//
class TransactionQueueHandler :
    public BlockingMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus BlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, AbstractLedger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( ledger );
        UNUSED ( jsonIn );
    
        string accountName = this->getMatchString ( "accountName" );
    
        SerializableVector < string > summary;
    
        TransactionQueue& transactionQueue = this->mWebMiner->getTransactionQueue ();
        const MakerQueue* makerQueue = transactionQueue.getMakerQueueOrNull ( accountName );
        
        if ( makerQueue ) {
        
            const MakerQueue::Queue& queue = makerQueue->getQueue ();
            MakerQueue::TransactionQueueConstIt queueIt = queue.cbegin ();
            for ( ; queueIt != queue.cend (); ++queueIt ) {
                shared_ptr < const Transaction > transaction = queueIt->second;
                summary.push_back ( transaction->getUUID ());
            }
        }
        
        jsonOut.set ( "queue", ToJSONSerializer::toJSON ( summary ));
        
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
