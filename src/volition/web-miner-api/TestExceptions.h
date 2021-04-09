// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_TESTEXCEPTIONS_H
#define VOLITION_WEBMINERAPI_TESTEXCEPTIONS_H

#include <volition/BlockingMinerAPIRequestHandler.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// TestExceptions
//================================================================//
class TestExceptions :
    public BlockingMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus BlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, AbstractLedger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( ledger );
        UNUSED ( jsonIn );
        UNUSED ( jsonOut );

        loguru::Text st = loguru::stacktrace ( 0 );

//        assert ( false );

        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
