// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_DEFAULTHANDLER_H
#define VOLITION_WEBMINERAPI_DEFAULTHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// DefaultHandler
//================================================================//
class DefaultHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        
        ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
        
        jsonOut.set ( "type", "VOL_MINING_NODE" );
        jsonOut.set ( "minerID", scopedLock.getWebMiner ().getMinerID ().c_str ());

        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
