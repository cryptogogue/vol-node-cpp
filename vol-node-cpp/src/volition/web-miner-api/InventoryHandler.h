// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_INVENTORYHANDLER_H
#define VOLITION_WEBMINERAPI_INVENTORYHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// InventoryHandler
//================================================================//
class InventoryHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        try {
        
            string accountName = this->getMatchString ( "accountName" );
        
            ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
            const Ledger& ledger = scopedLock.getWebMiner ().getLedger ();
        
            Schema schema;
            ledger.getSchema ( schema );
        
            SerializableList < SerializableSharedPtr < Asset >> inventory;
            ledger.getInventory ( schema, ledger.getAccountIndex ( accountName ), inventory, 1024, true ); // TODO: fix hardcoded page size
        
            Poco::Dynamic::Var inventoryJSON = ToJSONSerializer::toJSON ( inventory );
        
            jsonOut.set ( "inventory",  inventoryJSON.extract < Poco::JSON::Array::Ptr >());
            jsonOut.set ( "version",    ToJSONSerializer::toJSON ( schema.getVersion ()));
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
