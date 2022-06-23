// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_ACCOUNTLOGHANDLER_H
#define VOLITION_WEBMINERAPI_ACCOUNTLOGHANDLER_H

#include <volition/AccountODBM.h>
#include <volition/Block.h>
#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/Transaction.h>
#include <volition/TransactionStatus.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// AccountLogHandler
//================================================================//
class AccountLogHandler :
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
                
        u64 logSize = 0;
        Poco::JSON::Array::Ptr entriesJSON = new Poco::JSON::Array ();
        
        AccountODBM accountODBM ( ledger, accountName );
        if ( accountODBM ) {
        
            logSize = accountODBM.mAccountLogSize.get ();
        
            u64 base    = this->optQuery ( "base", 0 );
            u64 height  = base + BATCH_SIZE;
            height      = logSize < height ? logSize : height;
            
            shared_ptr < const Block > block;
            
            for ( u64 i = base; i < height; ++i ) {
        
                shared_ptr < const AccountLogEntry > entry = accountODBM.getAccountLogEntryField ( i ).get ();
                if ( !entry ) break;
                
                if ( !block || ( block->getHeight () != entry->getBlockHeight ())) {
                    block = ledger.getBlock ( entry->getBlockHeight ());
                }
                
                if ( !block ) break;
                
                const Transaction* transaction = block ? block->getTransaction ( entry->getTransactionIndex ()) : NULL;
                if ( transaction ) {
                    
                    AccountID accountID = ledger.getAccountID ( transaction->getMakerAccountName ());
                                        
                    Poco::JSON::Object::Ptr json = ToJSONSerializer::toJSON ( *transaction ).extract < Poco::JSON::Object::Ptr >();
                    json->set ( "makerIndex", ( u64 )accountID );
                    
                    TransactionDetailsPtr details = transaction->getDetails ( ledger );
                    if ( details ) {
                        json->set ( "details", ToJSONSerializer::toJSON ( *details ));
                    }
                    
                    Poco::JSON::Object::Ptr entryJSON = new Poco::JSON::Object ();
                    
                    entryJSON->set ( "time", ( string )SerializableTime ( block->getTime ()));
                    entryJSON->set ( "blockHeight", entry->getBlockHeight ());
                    entryJSON->set ( "transaction", json );
                    
                    entriesJSON->add ( *entryJSON );
                }
            }
        }
        jsonOut.set ( "entries",    entriesJSON );
        jsonOut.set ( "logSize",    logSize );
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
