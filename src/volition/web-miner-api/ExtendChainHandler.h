// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_EXTENDCHAINHANDLER_H
#define VOLITION_WEBMINERAPI_EXTENDCHAINHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// ExtendChainHandler
//================================================================//
class ExtendChainHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_POST )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
    
        try {        
            ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
            WebMiner& webMiner = scopedLock.getWebMiner ();
            
            bool lazy = webMiner.getLazy ();
            webMiner.setLazy ( false );
            webMiner.extendChain ();
            webMiner.setLazy ( lazy );
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
