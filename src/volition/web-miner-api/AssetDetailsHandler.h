// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_ASSETDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_ASSETDETAILSHANDLER_H

#include <volition/Asset.h>
#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// AssetDetailsHandler
//================================================================//
class AssetDetailsHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
    
        try {
        
            Asset::Index index = this->getMatchU64 ( "index" );
            
            ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
            const Ledger& ledger = scopedLock.getWebMiner ().getLedger ();
        
            shared_ptr < Asset > asset = ledger.getAsset ( index );
            
            if ( asset ) {
                Poco::Dynamic::Var assetJSON = ToJSONSerializer::toJSON ( *asset );
                jsonOut.set ( "asset", assetJSON.extract < Poco::JSON::Object::Ptr >());
            }
        }
        catch ( ... ) {
            return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
        }
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif