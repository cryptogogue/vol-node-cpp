// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_RESETCHAINHANDLER_H
#define VOLITION_WEBMINERAPI_RESETCHAINHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/Schema.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// ResetChainHandler
//================================================================//
class ResetChainHandler :
    public AbstractAPIRequestHandler {
public:

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleDelete () const override {
    
        try {
            ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
            scopedLock.getWebMiner ().reset ();
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
