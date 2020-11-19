// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_INVENTORYASSETSHANDLER_H
#define VOLITION_WEBMINERAPI_INVENTORYASSETSHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/MinerAPIFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// InventoryAssetsHandler
//================================================================//
class InventoryAssetsHandler :
    public MinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        try {
        
            string accountName = this->getMatchString ( "accountName" );
        
            ScopedMinerLock scopedLock ( this->mWebMiner );
            Ledger& ledger = this->mWebMiner->getHighConfidenceLedger ();
            
            SerializableList < SerializableSharedConstPtr < Asset >> inventory;
            ledger.getInventory ( ledger.getAccountID ( accountName ), inventory, 0, true );
        
            Poco::Dynamic::Var inventoryJSON = ToJSONSerializer::toJSON ( inventory );
        
            jsonOut.set ( "inventory",      inventoryJSON.extract < Poco::JSON::Array::Ptr >());
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
