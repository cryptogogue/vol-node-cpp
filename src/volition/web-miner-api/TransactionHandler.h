// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_TRANSACTIONHANDLER_H
#define VOLITION_WEBMINERAPI_TRANSACTIONHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// TransactionHandler
//================================================================//
class TransactionHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_POST )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {

        JSONSerializableTypeInfo typeInfo ( jsonIn );
        TransactionFactory factory;
        unique_ptr < AbstractTransaction > transaction = factory.make ( typeInfo );
        if ( transaction ) {
            FromJSONSerializer::fromJSON ( *transaction, jsonIn );
            
            ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
            scopedLock.getWebMiner ().pushTransaction ( move ( transaction ));
            
            return Poco::Net::HTTPResponse::HTTP_OK;
        }
        return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
