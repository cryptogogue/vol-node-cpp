// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_CONSENSUSBLOCKDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_CONSENSUSBLOCKDETAILSHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/MinerAPIFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// ConsensusBlockDetailsHandler
//================================================================//
class ConsensusBlockDetailsHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, Ledger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( ledger );
        UNUSED ( jsonIn );

        try {
        
            string hash = this->getMatchString ( "hash" );

            const AbstractBlockTree& blockTree = this->mWebMiner->getBlockTree ();
            BlockTreeCursor cursor = blockTree.findCursorForHash ( hash );
            shared_ptr < const Block > block = cursor.getBlock ();

            if ( block ) {
                jsonOut.set ( "block", ToJSONSerializer::toJSON ( *block ));
            }
            return Poco::Net::HTTPResponse::HTTP_OK;
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
