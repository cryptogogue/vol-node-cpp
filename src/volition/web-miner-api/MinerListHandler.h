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
    public AbstractMinerAPIRequestHandler {
public:

    static const size_t RANDOM_BATCH_SIZE = 16;

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, Ledger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( ledger );
        UNUSED ( jsonIn );
            
        set < string > minerURLs = ( this->optQuery ( "sample", "" ) == "random" ) ? this->mWebMiner->sampleOnlineMinerURLs ( RANDOM_BATCH_SIZE ) : this->mWebMiner->sampleOnlineMinerURLs ();
        
        SerializableList < string > result;
        set < string >::const_iterator urlIt = minerURLs.cbegin ();
        for ( ; urlIt != minerURLs.cend (); ++urlIt ) {
            result.push_back ( *urlIt );
        }
        
        jsonOut.set ( "miners", ToJSONSerializer::toJSON ( result ));
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
