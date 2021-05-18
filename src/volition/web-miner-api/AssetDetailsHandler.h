// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_ASSETDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_ASSETDETAILSHANDLER_H

#include <volition/Asset.h>
#include <volition/Block.h>
#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// AssetDetailsHandler
//================================================================//
class AssetDetailsHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        
        ScopedSharedMinerLedgerLock ledger ( miner );
        ledger.seek ( this->optQuery ( "at", ledger.countBlocks ()));
        
        string assetIndexOrID = this->getMatchString ( "assetIndexOrID" );
        if ( assetIndexOrID.size () == 0 ) return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
        
        u64 index = isdigit ( assetIndexOrID [ 0 ]) ? stoull ( assetIndexOrID ) : AssetID::decode ( assetIndexOrID );
        
        if ( index == AssetID::NULL_INDEX ) return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                
        shared_ptr < const Asset > asset = AssetODBM ( ledger, index ).getAsset ();
        
        if ( asset ) {
            Poco::Dynamic::Var assetJSON = ToJSONSerializer::toJSON ( *asset );
            jsonOut.set ( "asset", assetJSON.extract < Poco::JSON::Object::Ptr >());
            return Poco::Net::HTTPResponse::HTTP_OK;
        }
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
