// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_OFFERDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_OFFERDETAILSHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/OfferODBM.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// OfferDetailsHandler
//================================================================//
class OfferDetailsHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        
        ScopedSharedMinerLedgerLock ledger ( miner );
        ledger.seek ( this->optQuery ( "at", ledger.countBlocks ()));
        
        AssetID assetID = this->getMatchString ( "assetID" );
        if ( assetID.mIndex == AssetID::NULL_INDEX ) return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
        
        AssetODBM assetODBM ( ledger, assetID );
        if ( !assetODBM ) return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
        
        OfferID offerID = assetODBM.mOffer.get ();
        if ( offerID.mIndex == AssetID::NULL_INDEX ) {
        
            jsonOut.set ( "status", "NOT_FOR_SALE" );
        
            SerializableList < SerializableSharedConstPtr < Asset >> assets;
            shared_ptr < const Asset > asset = assetODBM.getAsset ();
            if ( !asset ) return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
            assets.push_back ( asset );
        
            jsonOut.set ( "assets", ToJSONSerializer::toJSON ( assets ));
            return Poco::Net::HTTPResponse::HTTP_OK;
        }
        
        OfferODBM offerODBM ( ledger, assetODBM.mOffer.get ());
        if ( !offerODBM ) return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
        
        AccountODBM sellerODBM ( ledger, offerODBM.mSeller.get ());
        if ( !sellerODBM ) return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
        
        jsonOut.set ( "status", "FOR_SALE" );
        
        jsonOut.set ( "offerID",        ( u64 )offerODBM.mOfferID );
        jsonOut.set ( "seller",         sellerODBM.mName.get ());
        jsonOut.set ( "minimumPrice",   offerODBM.mMinimumPrice.get ());
        jsonOut.set ( "expiration",     offerODBM.mExpiration.get ());
        
        SerializableList < SerializableSharedConstPtr < Asset >> assets;
        SerializableVector < AssetID::Index > assetIDs;
        offerODBM.mAssetIdentifiers.get ( assetIDs );
            
        SerializableVector < AssetID::Index >::const_iterator assetIDIt = assetIDs.cbegin ();
        for ( ; assetIDIt != assetIDs.cend (); ++assetIDIt ) {
        
            shared_ptr < const Asset > asset = AssetODBM ( ledger, *assetIDIt ).getAsset ();
            if ( !asset ) return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
            
            assets.push_back ( asset );
        }
        jsonOut.set ( "assets", ToJSONSerializer::toJSON ( assets ));
        
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
