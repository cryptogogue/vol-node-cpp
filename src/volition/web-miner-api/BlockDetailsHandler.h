// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_BLOCKDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_BLOCKDETAILSHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractMinerAPIRequestHandler.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// BlockDetailsHandler
//================================================================//
class BlockDetailsHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        
        ScopedSharedMinerLedgerLock ledger ( miner );
        ledger.seek ( this->optQuery ( "at", ledger.countBlocks ()));
        
        u64 height = this->getMatchU64 ( "blockID" );

        shared_ptr < const Block > block = ledger.getBlock ( height );
        if ( block ) {
            jsonOut.set ( "block", ToJSONSerializer::toJSON ( *block ));
            return Poco::Net::HTTPResponse::HTTP_OK;
        }
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
