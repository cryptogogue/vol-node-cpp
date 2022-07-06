// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_TERMSOFSERVICEHANDLER_H
#define VOLITION_WEBMINERAPI_TERMSOFSERVICEHANDLER_H

#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/Block.h>
#include <volition/ContractWithDigest.h>
#include <volition/Schema.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// TermsOfServiceHandler
//================================================================//
class TermsOfServiceHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        
        ScopedSharedMinerLedgerLock ledger ( miner );
        ledger.seek ( this->optQuery ( "at", ledger.countBlocks ()));
        
        ContractWithDigest contract = ledger.getTermsOfService ();

        jsonOut.set ( "contract",       ToJSONSerializer::toJSON ( contract ));

        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
