// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_CONSENSUSBLOCKDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_CONSENSUSBLOCKDETAILSHANDLER_H

#include <volition/AbstractConsensusInspector.h>
#include <volition/AbstractMinerAPIRequestHandler.h>
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
    public AbstractConsensusInspectorAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractConsensusInspectorAPIRequestHandler_handleRequest ( HTTP::Method method, AbstractConsensusInspector& inspector, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );

        try {
        
            string hash = this->getMatchString ( "hash" );

            shared_ptr < const Block > block = inspector.getBlock ( hash );

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
