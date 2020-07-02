// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_ASSETDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_ASSETDETAILSHANDLER_H

#include <volition/Asset.h>
#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// AssetDetailsHandler
//================================================================//
class AssetDetailsHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        try {
        
            string assetIndexOrID = this->getMatchString ( "assetIndexOrID" );
            assert ( assetIndexOrID.size () > 0 );
            
            u64 index = isdigit ( assetIndexOrID [ 0 ]) ? stoull ( assetIndexOrID ) : AssetID::decode ( assetIndexOrID );
            
            ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
            Ledger& ledger = scopedLock.getWebMiner ().getLedger ();
            const Schema& schema = ledger.getSchema ();
        
            shared_ptr < Asset > asset = ledger.getAsset ( schema, index );
            
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
