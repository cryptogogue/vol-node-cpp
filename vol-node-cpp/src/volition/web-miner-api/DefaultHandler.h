// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_DEFAULTHANDLER_H
#define VOLITION_WEBMINERAPI_DEFAULTHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/Format.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/TheWebMiner.h>
#include <volition/version.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// DefaultHandler
//================================================================//
class DefaultHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        
        ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
        
        jsonOut.set ( "type", "VOL_MINING_NODE" );
        jsonOut.set ( "minerID",    scopedLock.getWebMiner ().getMinerID ().c_str ());
        jsonOut.set ( "genesis",    scopedLock.getWebMiner ().getLedger ().getGenesisHash ());
        jsonOut.set ( "identity",   scopedLock.getWebMiner ().getLedger ().getIdentity ());
        jsonOut.set ( "build",      Format::write ( "%s %s", VOLITION_BUILD_DATE_STR, VOLITION_GIT_TAG_STR ));
        jsonOut.set ( "commit",     Format::write ( "%s", VOLITION_GIT_COMMIT_STR ));

        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
