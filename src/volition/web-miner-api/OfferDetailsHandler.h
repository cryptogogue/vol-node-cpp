// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_OFFERDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_OFFERDETAILSHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/OfferODBM.h>
#include <volition/TheTransactionBodyFactory.h>

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
        
        OfferID offerID = this->getMatchU64 ( "offerID" );
        if ( offerID.mIndex == OfferID::NULL_INDEX ) return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
        
        OfferODBM offerODBM ( ledger, offerID );
        if ( !offerODBM ) return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
        
        AccountODBM sellerODBM ( ledger, offerODBM.mSeller.get ());
        if ( !sellerODBM ) return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
        
        jsonOut.set ( "seller",         sellerODBM.mName.get ());
        jsonOut.set ( "minimumPrice",   offerODBM.mMinimumPrice.get ());
        jsonOut.set ( "expiration",     offerODBM.mExpiration.get ());
        
        shared_ptr < const SerializableVector < AssetID::Index >> assetIDsPtr = offerODBM.mAssetIdentifiers.get ();
        SerializableVector < AssetID::Index > assetIDs;
        if ( assetIDsPtr ) {
            assetIDs = *assetIDsPtr;
        }
        jsonOut.set ( "assetIDs", ToJSONSerializer::toJSON ( assetIDs ).extract < Poco::JSON::Array::Ptr >());
        
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
