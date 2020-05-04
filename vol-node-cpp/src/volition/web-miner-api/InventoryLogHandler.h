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
        
            ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
            const Ledger& ledger = scopedLock.getWebMiner ().getLedger ();
        
            shared_ptr < InventoryLogEntry > logEntry = ledger.getInventoryLogEntry ( accountName, nonce );
            if ( logEntry ) {
            
                Schema schema;
                ledger.getSchema ( schema );
                
                SerializableList < SerializableSharedPtr < Asset >> assets;
                logEntry->expandAdditions ( ledger, schema, accountName, assets );
                jsonOut.set ( "assets", ToJSONSerializer::toJSON ( assets ));
                
                SerializableList < string > additions;
                logEntry->decodeAdditions ( additions );
                jsonOut.set ( "additions", ToJSONSerializer::toJSON ( additions ));
                
                SerializableList < string > deletions;
                logEntry->decodeDeletions ( deletions );
                jsonOut.set ( "deletions", ToJSONSerializer::toJSON ( deletions ));
            }
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
