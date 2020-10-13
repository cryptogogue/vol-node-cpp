// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_ASSETDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_ASSETDETAILSHANDLER_H

#include <volition/Asset.h>
#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/MinerAPIFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// AssetDetailsHandler
//================================================================//
class AssetDetailsHandler :
    public MinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        try {
        
            string assetIndexOrID = this->getMatchString ( "assetIndexOrID" );
            if ( assetIndexOrID.size () == 0 ) return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
            
            u64 index = isdigit ( assetIndexOrID [ 0 ]) ? stoull ( assetIndexOrID ) : AssetID::decode ( assetIndexOrID );
            
            if ( index == AssetID::NULL_INDEX ) return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
            
            ScopedMinerLock scopedLock ( this->mWebMiner );
            Ledger& ledger = this->mWebMiner->getLedger ();
            const Schema& schema = ledger.getSchema ();
        
            shared_ptr < const Asset > asset = AssetODBM ( ledger, index ).getAsset ( schema );
            
            if ( asset ) {
                Poco::Dynamic::Var assetJSON = ToJSONSerializer::toJSON ( *asset );
                jsonOut.set ( "asset", assetJSON.extract < Poco::JSON::Object::Ptr >());
            }
            else {
                return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
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
