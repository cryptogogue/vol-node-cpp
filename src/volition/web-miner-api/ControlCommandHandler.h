// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_CONTROLCOMMANDHANDLER_H
#define VOLITION_WEBMINERAPI_CONTROLCOMMANDHANDLER_H

#include <volition/Block.h>
#include <volition/BlockingMinerAPIRequestHandler.h>
#include <volition/ControlCommand.h>
#include <volition/LedgerResult.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// ControlCommandHandler
//================================================================//
class ControlCommandHandler :
    public BlockingMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET_POST )

    //----------------------------------------------------------------//
    HTTPStatus BlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, AbstractLedger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( ledger );
        
        SerializableUniquePtr < ControlCommand > command;
        FromJSONSerializer::fromJSON ( command, jsonIn );

        LedgerResult result = command->execute ( *this->mWebMiner );

        jsonOut.set ( "status", result ? "OK" : "ERROR" );
        jsonOut.set ( "message", result.getMessage ());

        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
