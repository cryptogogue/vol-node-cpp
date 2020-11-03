// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_INVENTORYLOGHANDLER_H
#define VOLITION_WEBMINERAPI_INVENTORYLOGHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/InventoryLogEntry.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/MinerAPIFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// InventoryLogHandler
//================================================================//
class InventoryLogHandler :
    public MinerAPIRequestHandler {
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
            
            ScopedMinerLock scopedLock ( this->mWebMiner );
            Ledger& ledger = this->mWebMiner->getLedger ();
            
            SerializableSet < AssetID::Index > additions;
            SerializableSet < AssetID::Index > deletions;
            
            AccountODBM accountODBM ( ledger, accountName );
            
            for ( u64 i = 0; i < count; ++i ) {
                
                shared_ptr < const InventoryLogEntry > logEntry = accountODBM.getInventoryLogEntryField ( nonce + i ).get ();
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
