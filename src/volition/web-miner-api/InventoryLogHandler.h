// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_INVENTORYLOGHANDLER_H
#define VOLITION_WEBMINERAPI_INVENTORYLOGHANDLER_H

#include <volition/Block.h>
#include <volition/InventoryLogEntry.h>
#include <volition/SemiBlockingMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// InventoryLogHandler
//================================================================//
class InventoryLogHandler :
    public SemiBlockingMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus SemiBlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, AbstractLedger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        static const size_t ASSET_BATCH_SIZE = 256;
        
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
            if ( additions.size () >= ASSET_BATCH_SIZE ) break;
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
