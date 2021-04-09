// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_RESETCHAINHANDLER_H
#define VOLITION_WEBMINERAPI_RESETCHAINHANDLER_H

#include <volition/Block.h>
#include <volition/BlockingMinerAPIRequestHandler.h>
#include <volition/Schema.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// ResetChainHandler
//================================================================//
class ResetChainHandler :
    public BlockingMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::DELETE )

    //----------------------------------------------------------------//
    HTTPStatus BlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method, Ledger& ledger, const Poco::JSON::Object&, Poco::JSON::Object& ) const override {
        UNUSED ( ledger );
    
        ScopedMinerLock scopedLock ( this->mWebMiner );
        this->mWebMiner->reset ();

        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
