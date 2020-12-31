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
            BlockTreeCursor cursor = this->mWebMiner->getWorkingLedgerTag ();
                        
            size_t top = this->optQuery ( "height", ( *cursor ).getHeight ()) + 1; // this handles "forward" and "backward" cases alike
            size_t base = HEADER_BATCH_SIZE < top ? top - HEADER_BATCH_SIZE : 0;
                        
            SerializableList < SerializableSharedConstPtr < BlockHeader >> headers;
            while ( cursor.asBool () && ( base <= ( *cursor ).getHeight ())) {
                if (( *cursor ).getHeight () < top ) {
                    shared_ptr < const BlockHeader > header = make_shared < BlockHeader >( *cursor.getBlockHeader ());
                    headers.push_front ( header );
                }
                cursor = cursor.getParent ();
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
