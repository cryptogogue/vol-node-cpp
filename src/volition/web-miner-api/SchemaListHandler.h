// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_SCHEMALISTHANDLER_H
#define VOLITION_WEBMINERAPI_SCHEMALISTHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/Schema.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// SchemaListHandler
//================================================================//
class SchemaListHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        try {
            ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
            const Ledger& ledger = scopedLock.getWebMiner ().getLedger ();
        
            list < Schema > schemas = ledger.getSchemas ();
            list < Schema >::const_iterator schemaIt = schemas.cbegin ();
            
            Poco::JSON::Array::Ptr jsonArray = new Poco::JSON::Array ();
            for ( int i = 0; schemaIt != schemas.cend (); ++i, ++schemaIt ) {
                const Schema& schema = *schemaIt;
                jsonArray->set ( i, ToJSONSerializer::toJSON ( schema ));
            }
            
            jsonOut.set ( "schemas", jsonArray );
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
