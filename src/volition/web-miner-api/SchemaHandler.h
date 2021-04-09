// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_SCHEMAHANDLER_H
#define VOLITION_WEBMINERAPI_SCHEMAHANDLER_H

#include <volition/Block.h>
#include <volition/Schema.h>
#include <volition/SemiBlockingMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// SchemaHandler
//================================================================//
class SchemaHandler :
    public SemiBlockingMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus SemiBlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, AbstractLedger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        
        const Schema& schema = ledger.getSchema ();

        jsonOut.set ( "schema",         ToJSONSerializer::toJSON ( schema ));
        jsonOut.set ( "schemaHash",     ledger.getSchemaHash ());

        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
