// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_CONSENSUSBLOCKHEADERLISTHANDLER_H
#define VOLITION_WEBMINERAPI_CONSENSUSBLOCKHEADERLISTHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/MinerAPIFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// ConsensusBlockHeaderListHandler
//================================================================//
class ConsensusBlockHeaderListHandler :
    public MinerAPIRequestHandler {
public:

    static const size_t HEADER_BATCH_SIZE = 32;

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );

        try {
        
            ScopedMinerLock scopedLock ( this->mWebMiner );
            BlockTreeNode::ConstPtr node = this->mWebMiner->getBestBranch ();
            
            size_t top = this->optQuery ( "height", ( **node ).getHeight ());
            size_t base = HEADER_BATCH_SIZE < top ? top - HEADER_BATCH_SIZE : 0;
            
            SerializableList < SerializableSharedConstPtr < BlockHeader >> headers;
            while ( node && ( base <= ( **node ).getHeight ())) {
                if (( **node ).getHeight () < top ) {
                    headers.push_front ( node->getBlockHeader ());
                }
                node = node->getParent ();
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
