// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_TRANSACTIONQUEUEHANDLER_H
#define VOLITION_WEBMINERAPI_TRANSACTIONQUEUEHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/TransactionEnvelope.h>
#include <volition/TransactionStatus.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// TransactionQueueHandler
//================================================================//
class TransactionQueueHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        ScopedExclusiveMinerLock minerLock ( miner );
    
        string accountName = this->getMatchString ( "accountName" );
    
        SerializableVector < string > summary;
    
        TransactionQueue& transactionQueue = miner->getTransactionQueue ();
        const TransactionMakerQueue* makerQueue = transactionQueue.getMakerQueueOrNull ( accountName );
        
        if ( makerQueue ) {
        
            const TransactionMakerQueue::Queue& queue = makerQueue->getQueue ();
            TransactionMakerQueue::TransactionQueueConstIt queueIt = queue.cbegin ();
            for ( ; queueIt != queue.cend (); ++queueIt ) {
                shared_ptr < const TransactionEnvelope > transaction = queueIt->second;
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
