// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_INVENTORYASSETSHANDLER_H
#define VOLITION_WEBMINERAPI_INVENTORYASSETSHANDLER_H

#include <volition/Block.h>
#include <volition/SemiBlockingMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// InventoryAssetsHandler
//================================================================//
class InventoryAssetsHandler :
    public SemiBlockingMinerAPIRequestHandler {
public:

    static const size_t ASSET_BATCH_SIZE = 256;

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus SemiBlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, LockedLedgerIterator& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        try {
        
            string accountName = this->getMatchString ( "accountName" );
            size_t base = ( size_t )this->optQuery ( "base", 0 );
            
            SerializableList < SerializableSharedConstPtr < Asset >> inventory;
            ledger.getInventory ( ledger.getAccountID ( accountName ), inventory, base, ASSET_BATCH_SIZE );
        
            Poco::Dynamic::Var inventoryJSON = ToJSONSerializer::toJSON ( inventory );
        
            jsonOut.set ( "inventory", inventoryJSON.extract < Poco::JSON::Array::Ptr >());
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
