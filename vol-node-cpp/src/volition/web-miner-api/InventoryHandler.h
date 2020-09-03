// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_INVENTORYHANDLER_H
#define VOLITION_WEBMINERAPI_INVENTORYHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/InventoryLogEntry.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/WebMinerAPIFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// InventoryHandler
//================================================================//
class InventoryHandler :
    public WebMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        try {
        
            string accountName = this->getMatchString ( "accountName" );
        
            ScopedWebMinerLock scopedLock ( this->mWebMiner );
            Ledger& ledger = this->mWebMiner->getLedger ();
        
            Account::Index accountIndex = ledger.getAccountIndex ( accountName );
            
            AccountODBM accountODBM ( ledger, accountIndex );
            jsonOut.set ( "assetCount", accountODBM.mAssetCount.get ( 0 ));
            jsonOut.set ( "inventoryNonce", accountODBM.mInventoryNonce.get ( 0 ));
        
            shared_ptr < InventoryLogEntry > baseEntry = ledger.getInventoryLogEntry ( accountIndex, 0 );
            if ( baseEntry ) {
                jsonOut.set ( "inventoryTimestamp", ( string )baseEntry->mTime );
            }
            
            const Schema& schema = ledger.getSchema ();
            jsonOut.set ( "schemaVersion",  ToJSONSerializer::toJSON ( schema.getVersion ()));
            jsonOut.set ( "schemaHash",     ledger.getSchemaHash ());
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
