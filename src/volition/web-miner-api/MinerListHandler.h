// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_MINERLISTHANDLER_H
#define VOLITION_WEBMINERAPI_MINERLISTHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// MinerListHandler
//================================================================//
class MinerListHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
        const Ledger& ledger = scopedLock.getWebMiner ().getLedger ();
        const map < string, MinerInfo >& minerInfoMap = ledger.getMiners ();
        
        Poco::JSON::Object::Ptr minersJSON = new Poco::JSON::Object ();
        
        map < string, MinerInfo >::const_iterator minerInfoIt = minerInfoMap.cbegin ();
        for ( unsigned int i = 0; minerInfoIt != minerInfoMap.cend (); ++minerInfoIt, ++i ) {
            const MinerInfo& minerInfo = minerInfoIt->second;
            
            Poco::JSON::Object::Ptr minerInfoJSON = new Poco::JSON::Object ();
            minerInfoJSON->set ( "url",         minerInfo.getURL ().c_str ());
            minersJSON->set ( minerInfo.getMinerID (), minerInfoJSON );
        }
        
        jsonOut.set ( "miners", minersJSON );
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
