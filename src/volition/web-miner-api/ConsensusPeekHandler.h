// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_CONSENSUSPEEKHANDLER_H
#define VOLITION_WEBMINERAPI_CONSENSUSPEEKHANDLER_H

#include <volition/Block.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/SemiBlockingMinerAPIRequestHandler.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// ConsensusPeekHandler
//================================================================//
class ConsensusPeekHandler :
    public SemiBlockingMinerAPIRequestHandler {
public:

    static const size_t HEADER_BATCH_SIZE = 32;

    SUPPORTED_HTTP_METHODS ( HTTP::GET )
    
    //----------------------------------------------------------------//
    void peek ( AbstractLedger& ledger, Poco::JSON::Object& jsonOut, string key, u64 height, u64 totalBlocks ) const {
    
        LGN_LOG_SCOPE ( VOL_FILTER_HTTP, INFO, __PRETTY_FUNCTION__ );
    
        if ( height < totalBlocks ) {
    
            Poco::JSON::Object::Ptr headerJSON = new Poco::JSON::Object ();
            
            shared_ptr < const BlockHeader > header = ledger.getHeader ( height );
            headerJSON->set ( "height", header->getHeight ());
            headerJSON->set ( "digest", header->getDigest ().toHex ());
            
            jsonOut.set ( key, headerJSON );
        }
    }

    //----------------------------------------------------------------//
    HTTPStatus SemiBlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, AbstractLedger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
            
        LGN_LOG_SCOPE ( VOL_FILTER_HTTP, INFO, __PRETTY_FUNCTION__ );
        
        LGN_LOG ( VOL_FILTER_HTTP, INFO, "getting miner ID" );
        jsonOut.set ( "minerID", this->mSnapshot.getMinerID ());
        
        LGN_LOG ( VOL_FILTER_HTTP, INFO, "counting blocks" );
        size_t totalBlocks = ledger.countBlocks ();
        jsonOut.set ( "totalBlocks", totalBlocks );

        LGN_LOG ( VOL_FILTER_HTTP, INFO, "peeking" );
        this->peek ( ledger, jsonOut, "peek", this->optQuery ( "peek", 0 ), totalBlocks );
        this->peek ( ledger, jsonOut, "prev", this->optQuery ( "prev", 0 ), totalBlocks );
        
        u64 sampleMiners = this->optQuery ( "sampleMiners", 0 );
        
        if ( sampleMiners ) {
        
            LGN_LOG ( VOL_FILTER_HTTP, INFO, "sampling miners" );
            set < string > minerURLs = this->mSnapshot.sampleOnlineMinerURLs ( sampleMiners );
            Poco::JSON::Array::Ptr minerURLsJSON = new Poco::JSON::Array ();
        
            LGN_LOG ( VOL_FILTER_HTTP, INFO, "serializing sampled miners" );
            set < string >::const_iterator urlIt = minerURLs.cbegin ();
            for ( ; urlIt != minerURLs.cend (); ++urlIt ) {
                minerURLsJSON->add ( *urlIt );
            }
            jsonOut.set ( "miners", minerURLsJSON );
        }
        
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
