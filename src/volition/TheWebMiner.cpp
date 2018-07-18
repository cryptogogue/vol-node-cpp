// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/TheWebMinerAPI.h>
#include <volition/TheWebMiner.h>

namespace Volition {

//================================================================//
// TheWebMiner
//================================================================//

//----------------------------------------------------------------//
TheWebMiner::TheWebMiner () {
    
    this->mRouteTable.addEndpoint < TheWebMinerAPI::AccountDetailsHandler >     ( "/accounts/:accountName/?" );                 // GET
    this->mRouteTable.addEndpoint < TheWebMinerAPI::AccountKeyListHandler >     ( "/accounts/:accountName/keys/?" );            // GET
    this->mRouteTable.addEndpoint < TheWebMinerAPI::BlockDetailsHandler >       ( "/blocks/:blockID/?" );                       // GET
    this->mRouteTable.addEndpoint < TheWebMinerAPI::BlockListHandler >          ( "/blocks/?" );                                // GET
    this->mRouteTable.addEndpoint < TheWebMinerAPI::MinerListHandler >          ( "/miners/?" );                                // GET
    this->mRouteTable.addEndpoint < TheWebMinerAPI::TransactionHandler >        ( "/transactions/?" );                          // POST

    this->mRouteTable.setDefault < TheWebMinerAPI::DefaultHandler >             ();
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
Poco::Net::HTTPRequestHandler* TheWebMiner::createRequestHandler ( const Poco::Net::HTTPServerRequest& request ) {
    
    return this->mRouteTable.match ( request.getURI ());
}

} // namespace Volition
