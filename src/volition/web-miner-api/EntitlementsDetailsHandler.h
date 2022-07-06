// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_ENTITLEMENTSDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_ENTITLEMENTSDETAILSHANDLER_H

#include <volition/AccountODBM.h>
#include <volition/Block.h>
#include <volition/AbstractMinerAPIRequestHandler.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// EntitlementsDetailsHandler
//================================================================//
class EntitlementsDetailsHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        ScopedSharedMinerLedgerLock ledger ( miner );
        ledger.seek ( this->optQuery ( "at", ledger.countBlocks ()));
    
        string name = this->getMatchString ( "name" );
        string family = this->optQuery ( "family", "" );

        Entitlements entitlements;
        
        if ( family == "key" ) {
            entitlements = ledger.getEntitlementsWithFamily < KeyEntitlements >( name );
        }
        else if ( family == "account" ) {
            entitlements = ledger.getEntitlementsWithFamily < AccountEntitlements >( name );
        }
        else {
            entitlements = ledger.getEntitlements ( name );
        }

        jsonOut.set ( "entitlements", ToJSONSerializer::toJSON ( entitlements ));

        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
