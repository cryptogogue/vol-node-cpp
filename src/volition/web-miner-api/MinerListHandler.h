// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_MINERLISTHANDLER_H
#define VOLITION_WEBMINERAPI_MINERLISTHANDLER_H

#include <volition/AbstractMinerAPIRequestHandler.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// MinerListHandler
//================================================================//
class MinerListHandler :
    public AbstractMinerAPIRequestHandler {
public:

    static const size_t RANDOM_BATCH_SIZE = 16;

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        
        size_t batchSize = ( this->optQuery ( "sample", "" ) == "random" ) ? RANDOM_BATCH_SIZE : 0;
        
        ScopedSharedMinerStatusLock minerStatus ( miner );
        set < string > minerURLs = minerStatus.sampleOnlineMinerURLs ( batchSize );
        
        SerializableList < string > result;
        set < string >::const_iterator urlIt = minerURLs.cbegin ();
        for ( ; urlIt != minerURLs.cend (); ++urlIt ) {
            result.push_back ( *urlIt );
        }
        
        jsonOut.set ( "miners", ToJSONSerializer::toJSON ( result ));
        jsonOut.set ( "isMiner", minerStatus.mIsMiner );
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
