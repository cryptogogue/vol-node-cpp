// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_EXTENDCHAINHANDLER_H
#define VOLITION_WEBMINERAPI_EXTENDCHAINHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/MinerAPIFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// ExtendChainHandler
//================================================================//
class ExtendChainHandler :
    public MinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::POST )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        UNUSED ( jsonOut );
    
        try {
            ScopedMinerLock scopedLock ( this->mWebMiner );
            
            time_t now;
            time ( &now );
            
            bool lazy = this->mWebMiner->isLazy ();
            this->mWebMiner->setLazy ( false );
            this->mWebMiner->extend ( now );
            this->mWebMiner->setLazy ( lazy );
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
