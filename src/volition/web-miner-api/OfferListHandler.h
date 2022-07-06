// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_OFFERLISTHANDLER_H
#define VOLITION_WEBMINERAPI_OFFERLISTHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/OfferODBM.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// OfferListHandler
//================================================================//
class OfferListHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        
        ScopedSharedMinerLedgerLock ledger ( miner );
        ledger.seek ( this->optQuery ( "at", ledger.countBlocks ()));
        
        LedgerFieldODBM < u64 > globalOpenOfferCountField ( ledger, OfferODBM::keyFor_globalOpenOfferCount ());
        u64 totalOffers = globalOpenOfferCountField.get ( 0 );
        
        u64 base    = this->optQuery ( "base", 0 );
        u64 count   = this->optQuery ( "count", 10 );
        
        base        = min < u64 >( base, totalOffers - 1 );
        count       = min < u64 >( count, 10 );
        u64 top     = min < u64 >( base + count, totalOffers );
        
        Poco::JSON::Array::Ptr offerListJSON = new Poco::JSON::Array ();
        for ( u64 i = base; i < top; ++i ) {
            
            LedgerFieldODBM < u64 > globalOpenOfferListElementField ( ledger, OfferODBM::keyFor_globalOpenOfferListElement ( i ));
            
            OfferID offerID = globalOpenOfferListElementField.get ();
            OfferODBM offerODBM ( ledger, offerID );
            
            AccountODBM sellerODBM ( ledger, offerODBM.mSeller.get ());
            if ( !sellerODBM ) return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
            
            Poco::JSON::Object::Ptr offerJSON = new Poco::JSON::Object ();
            
            offerJSON->set ( "status",          "FOR_SALE" );
            offerJSON->set ( "offerID",         ( u64 )offerODBM.mOfferID );
            offerJSON->set ( "seller",          sellerODBM.mName.get ());
            offerJSON->set ( "minimumPrice",    offerODBM.mMinimumPrice.get ());
            offerJSON->set ( "expiration",      offerODBM.mExpiration.get ());
 
            offerListJSON->add ( *offerJSON );
        }
        
        jsonOut.set ( "offers", offerListJSON );
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
