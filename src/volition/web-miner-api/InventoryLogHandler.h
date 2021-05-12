// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_INVENTORYLOGHANDLER_H
#define VOLITION_WEBMINERAPI_INVENTORYLOGHANDLER_H

#include <volition/Block.h>
#include <volition/InventoryLogEntry.h>
#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// InventoryLogHandler
//================================================================//
class InventoryLogHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        ScopedSharedMinerLedgerLock ledger ( miner );
        ledger.seek ( this->optQuery ( "at", ledger.countBlocks ()));
            
        string accountName  = this->getMatchString ( "accountName" );
        u64 nonce           = this->getMatchU64 ( "nonce" );
        u64 count           = this->optQuery ( "count", 1 );
        
        SerializableSet < AssetID::Index > additions;
        SerializableSet < AssetID::Index > deletions;
        
        AccountODBM accountODBM ( ledger, accountName );
        
        u64 found = 0;
        for ( ; found < count; ++found ) {
            
            shared_ptr < const InventoryLogEntry > logEntry = accountODBM.getInventoryLogEntryField ( nonce + found ).get ();
            if ( !logEntry ) continue;
            
            logEntry->apply ( additions, deletions );
        }
        
        SerializableList < SerializableSharedConstPtr < Asset >> assets;
        InventoryLogEntry::expand ( ledger, accountName, additions, assets );
        jsonOut.set ( "assets", ToJSONSerializer::toJSON ( assets ));
        
        SerializableList < string > additionDecoded;
        InventoryLogEntry::decode ( additions, additionDecoded );
        jsonOut.set ( "additions", ToJSONSerializer::toJSON ( additionDecoded ));
        
        SerializableList < string > deletionsDecoded;
        InventoryLogEntry::decode ( deletions, deletionsDecoded );
        jsonOut.set ( "deletions", ToJSONSerializer::toJSON ( deletionsDecoded ));
        
        jsonOut.set ( "nextNonce", nonce + found );
        
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
