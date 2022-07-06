// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_IDENTITYPROVIDERDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_IDENTITYPROVIDERDETAILSHANDLER_H

#include <volition/AccountODBM.h>
#include <volition/AssetODBM.h>
#include <volition/Block.h>
#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/IdentityProvider.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// IdentityProviderDetailsHandler
//================================================================//
class IdentityProviderDetailsHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        
        ScopedSharedMinerLedgerLock ledger ( miner );
        ledger.seek ( this->optQuery ( "at", ledger.countBlocks ()));
        
        string providerName = this->getMatchString ( "provider" );
        jsonOut.set ( "provider", ToJSONSerializer::toJSON ( ledger.getIdentityProvider ( providerName )));
        
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
