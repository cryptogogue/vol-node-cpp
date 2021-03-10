// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_CONTROLCOMMANDHANDLER_H
#define VOLITION_WEBMINERAPI_CONTROLCOMMANDHANDLER_H

#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/ControlCommand.h>
#include <volition/LedgerResult.h>
#include <volition/MinerAPIFactory.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// ControlCommandHandler
//================================================================//
class ControlCommandHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET_POST )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, Ledger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
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
