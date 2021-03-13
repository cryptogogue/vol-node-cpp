// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_CONSENSUSBLOCKHEADERLISTHANDLER_H
#define VOLITION_WEBMINERAPI_CONSENSUSBLOCKHEADERLISTHANDLER_H

#include <volition/Block.h>
#include <volition/BlockingMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// ConsensusBlockHeaderListHandler
//================================================================//
class ConsensusBlockHeaderListHandler :
    public BlockingMinerAPIRequestHandler {
public:

    static const size_t HEADER_BATCH_SIZE = 256;

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus BlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, Ledger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );

        try {
                    
            size_t totalBlocks = ledger.countBlocks ();
            
            size_t max = this->optQuery ( "max", HEADER_BATCH_SIZE );
            
            size_t base = this->optQuery ( "height", 0 );
            base = base < totalBlocks ? base : totalBlocks - 1;
            
            size_t top = base + max;
            top = top <= totalBlocks ? top : totalBlocks;
            
            SerializableList < SerializableSharedConstPtr < BlockHeader >> headers;
            for ( size_t i = base; i < top; ++i ) {
                shared_ptr < const BlockHeader > header = ledger.getHeader ( i );
                if ( !header ) break;
                headers.push_back ( header );
            }
            
            jsonOut.set ( "headers", ToJSONSerializer::toJSON ( headers ));
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
