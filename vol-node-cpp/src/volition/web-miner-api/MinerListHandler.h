// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_MINERLISTHANDLER_H
#define VOLITION_WEBMINERAPI_MINERLISTHANDLER_H

#include <volition/AccountODBM.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/Block.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/MinerAPIFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// MinerListHandler
//================================================================//
class MinerListHandler :
    public MinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        ScopedMinerLock scopedLock ( this->mWebMiner );
        const Ledger& ledger = this->mWebMiner->getLedger ();
        set < string > miners = ledger.getMiners ();
        
        Poco::JSON::Object::Ptr minersJSON = new Poco::JSON::Object ();
        
        set < string >::const_iterator minerIt = miners.cbegin ();
        for ( unsigned int i = 0; minerIt != miners.cend (); ++minerIt, ++i ) {
        
            AccountODBM minerODBM ( ledger, *minerIt );
            if ( !minerODBM ) continue;
            
            Poco::JSON::Object::Ptr minerInfoJSON = new Poco::JSON::Object ();
            minerInfoJSON->set ( "url",         minerODBM.mMinerInfo.get ()->getURL ().c_str ());
            
            minersJSON->set ( *minerIt, minerInfoJSON );
        }
        
        jsonOut.set ( "miners", minersJSON );
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
