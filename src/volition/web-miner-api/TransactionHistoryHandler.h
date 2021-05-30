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
                
        u64 transactionLogSize = 0;
        Poco::JSON::Array::Ptr transactionsJSON = new Poco::JSON::Array ();
        
        AccountODBM accountODBM ( ledger, accountName );
        if ( accountODBM ) {
        
            transactionLogSize = accountODBM.mTransactionLogSize.get ();
        
            u64 base = this->optQuery ( "base", 0 );
            u64 height = base + BATCH_SIZE;
            height = transactionLogSize < height ? transactionLogSize : height;
            
            shared_ptr < const Block > block;
            
            for ( u64 i = base; i < height; ++i ) {
        
                shared_ptr < const TransactionLogEntry > entry = accountODBM.getTransactionLogEntryField ( i ).get ();
                if ( !entry ) break;
                
                if ( !block || ( block->getHeight () != entry->getBlockHeight ())) {
                    block = ledger.getBlock ( entry->getBlockHeight ());
                }
                
                if ( !block ) break;
                
                const Transaction* transaction = block ? block->getTransaction ( entry->getTransactionIndex ()) : NULL;
                if ( transaction ) {
                    
                    AccountID accountID = ledger.getAccountID ( transaction->getMaker ()->getAccountName ());
                                        
                    Poco::JSON::Object::Ptr json = ToJSONSerializer::toJSON ( *transaction ).extract < Poco::JSON::Object::Ptr >();
                    json->set ( "makerIndex", ( u64 )accountID );
                    transactionsJSON->add ( *json );
                }
            }
        }
        jsonOut.set ( "transactions",       transactionsJSON );
        jsonOut.set ( "totalTransactions",  transactionLogSize );
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
