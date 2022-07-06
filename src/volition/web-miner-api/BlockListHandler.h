// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_BLOCKLISTHANDLER_H
#define VOLITION_WEBMINERAPI_BLOCKLISTHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractMinerAPIRequestHandler.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// BlockListHandler
//================================================================//
class BlockListHandler :
    public AbstractMinerAPIRequestHandler {
public:
    
    static const size_t BLOCKS_PAGE_SIZE = 32;

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        
        ScopedSharedMinerLedgerLock ledger ( miner );
        ledger.seek ( this->optQuery ( "at", ledger.countBlocks ()));
        
        size_t totalBlocks = ledger.countBlocks ();
        
        size_t max = this->optQuery ( "max", BLOCKS_PAGE_SIZE );
        
        size_t base = this->optQuery ( "height", 0 );
        base = base < totalBlocks ? base : totalBlocks - 1;
        
        size_t top = base + max;
        top = top <= totalBlocks ? top : totalBlocks;
        
        SerializableList < SerializableSharedConstPtr < Block >> blocks;
        for ( size_t i = base; i < top; ++i ) {
            shared_ptr < const Block > block = ledger.getBlock ( i );
            if ( !block ) break;
            blocks.push_back ( block );
        }
        
        jsonOut.set ( "blocks", ToJSONSerializer::toJSON ( blocks ));
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
