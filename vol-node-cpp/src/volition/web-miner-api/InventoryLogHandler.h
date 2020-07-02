// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_INVENTORYLOGHANDLER_H
#define VOLITION_WEBMINERAPI_INVENTORYLOGHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/InventoryLogEntry.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// InventoryLogHandler
//================================================================//
class InventoryLogHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        try {
        
            string accountName  = this->getMatchString ( "accountName" );
            u64 nonce           = this->getMatchU64 ( "nonce" );
            u64 count           = this->optQuery ( "count", 1 );
            
            ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
            Ledger& ledger = scopedLock.getWebMiner ().getLedger ();
            const Schema& schema = ledger.getSchema ();
            
            SerializableSet < AssetID::Index > additions;
            SerializableSet < AssetID::Index > deletions;
            
            for ( u64 i = 0; i < count; ++i ) {
                
                shared_ptr < InventoryLogEntry > logEntry = ledger.getInventoryLogEntry ( ledger.getAccountIndex ( accountName ), nonce + i );
                if ( !logEntry ) continue;
                
                logEntry->apply ( additions, deletions );
            }
            
            SerializableList < SerializableSharedPtr < Asset >> assets;
            InventoryLogEntry::expand ( ledger, schema, accountName, additions, assets );
            jsonOut.set ( "assets", ToJSONSerializer::toJSON ( assets ));
            
            SerializableList < string > additionDecoded;
            InventoryLogEntry::decode ( additions, additionDecoded );
            jsonOut.set ( "additions", ToJSONSerializer::toJSON ( additionDecoded ));
            
            SerializableList < string > deletionsDecoded;
            InventoryLogEntry::decode ( deletions, deletionsDecoded );
            jsonOut.set ( "deletions", ToJSONSerializer::toJSON ( deletionsDecoded ));
        }
        catch ( ... ) {
            return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
        }
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
