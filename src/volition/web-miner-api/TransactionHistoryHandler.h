// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_TRANSACTIONHISTORYHANDLER_H
#define VOLITION_WEBMINERAPI_TRANSACTIONHISTORYHANDLER_H

#include <volition/AccountODBM.h>
#include <volition/Block.h>
#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/Transaction.h>
#include <volition/TransactionStatus.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// TransactionHistoryHandler
//================================================================//
class TransactionHistoryHandler :
    public AbstractMinerAPIRequestHandler {
public:

    static const size_t BATCH_SIZE = 4;

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        ScopedSharedMinerLedgerLock ledger ( miner );
        ledger.seek ( this->optQuery ( "at", ledger.countBlocks ()));
    
        string accountName = this->getMatchString ( "accountName" );
        
        SerializableVector < Transaction > transactions;
        
        AccountODBM accountODBM ( ledger, accountName );
        if ( accountODBM ) {
        
            u64 accountNonce = accountODBM.mTransactionNonce.get ();
            
            u64 height = accountNonce + BATCH_SIZE;
            height = accountNonce < height ? accountNonce : height;
            
            u64 nonce = this->getMatchU64 ( "nonce" );
            shared_ptr < const Block > block;
            
            for ( ; nonce < height; ++nonce ) {
        
                shared_ptr < const TransactionLogEntry > entry = accountODBM.getTransactionLogEntryField ( nonce ).get ();
                if ( !entry ) break;
                
                if ( !block || ( block->getHeight () != entry->getBlockHeight ())) {
                    block = ledger.getBlock ( entry->getBlockHeight ());
                }
                
                if ( !block ) break;
                
                const Transaction* transaction = block ? block->getTransaction ( entry->getTransactionIndex ()) : NULL;
                if ( transaction ) {
                    transactions.push_back ( *transaction );
                }
            }
        }
        jsonOut.set ( "transactions", ToJSONSerializer::toJSON ( transactions ));
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
