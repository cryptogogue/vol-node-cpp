// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_CONSENSUSBLOCKDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_CONSENSUSBLOCKDETAILSHANDLER_H

#include <volition/AbstractConsensusInspector.h>
#include <volition/Block.h>
#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>

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
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        
        string hash = this->getMatchString ( "hash" );

        ScopedSharedMinerStatusLock minerStatus ( miner );
        MinerSnapshot::InspectorPtr inspector = minerStatus.createInspector ();
        shared_ptr < const Block > block = inspector ? inspector->getBlock ( hash ) : NULL;

        if ( block ) {
            jsonOut.set ( "block", ToJSONSerializer::toJSON ( *block ));
        }
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
