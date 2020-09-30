// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_BLOCKDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_BLOCKDETAILSHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/MinerAPIFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// BlockDetailsHandler
//================================================================//
class BlockDetailsHandler :
    public MinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );

        try {
        
            u64 height = this->getMatchU64 ( "blockID" );

            ScopedMinerLock scopedLock ( this->mWebMiner );
            const Chain& chain = *this->mWebMiner->getChain ();

            shared_ptr < Block > block = chain.getBlock ( height );
            if ( block ) {
                jsonOut.set ( "block", ToJSONSerializer::toJSON ( *block ));
                return Poco::Net::HTTPResponse::HTTP_OK;
            }
        }
        catch ( ... ) {
            return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
        }
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
