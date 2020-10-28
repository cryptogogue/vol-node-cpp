// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_MINERLISTHANDLER_H
#define VOLITION_WEBMINERAPI_MINERLISTHANDLER_H

#include <volition/AccountODBM.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/Block.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/MinerAPIFactory.h>
#include <volition/UnsecureRandom.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// MinerListHandler
//================================================================//
class MinerListHandler :
    public MinerAPIRequestHandler {
public:

    static const size_t RANDOM_BATCH_SIZE = 16;

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        ScopedMinerLock scopedLock ( this->mWebMiner );
        set < string > miners = ( this->optQuery ( "sample", "" ) == "random" ) ? this->mWebMiner->sampleActiveMinerURLs ( RANDOM_BATCH_SIZE ) : this->mWebMiner->getActiveMinerURLs ();
        
        SerializableList < string > minerList;
        
        set < string >::const_iterator minerIt = miners.cbegin ();
        for ( ; minerIt != miners.cend (); ++minerIt ) {
            minerList.push_back ( *minerIt );
        }
        
        jsonOut.set ( "miners", ToJSONSerializer::toJSON ( minerList ));
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
