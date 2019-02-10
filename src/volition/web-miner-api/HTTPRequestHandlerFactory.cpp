// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/web-miner-api/AccountDetailsHandler.h>
#include <volition/web-miner-api/AccountKeyListHandler.h>
#include <volition/web-miner-api/BlockDetailsHandler.h>
#include <volition/web-miner-api/BlockListHandler.h>
#include <volition/web-miner-api/DefaultHandler.h>
#include <volition/web-miner-api/ExtendChainHandler.h>
#include <volition/web-miner-api/HTTPRequestHandlerFactory.h>
#include <volition/web-miner-api/InventoryHandler.h>
#include <volition/web-miner-api/KeyDetailsHandler.h>
#include <volition/web-miner-api/MinerListHandler.h>
#include <volition/web-miner-api/SchemaListHandler.h>
#include <volition/web-miner-api/TestKeyIDHandler.h>
#include <volition/web-miner-api/TransactionHandler.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// WebMinerAPI
//================================================================//

//----------------------------------------------------------------//
HTTPRequestHandlerFactory::HTTPRequestHandlerFactory () {

    this->mRouteTable.addEndpoint < WebMinerAPI::AccountDetailsHandler >    ( "/accounts/:accountName/?" );             // GET
    this->mRouteTable.addEndpoint < WebMinerAPI::InventoryHandler >         ( "/accounts/:accountName/inventory/?" );   // GET
    this->mRouteTable.addEndpoint < WebMinerAPI::AccountKeyListHandler >    ( "/accounts/:accountName/keys/?" );        // GET
    this->mRouteTable.addEndpoint < WebMinerAPI::BlockDetailsHandler >      ( "/blocks/:blockID/?" );                   // GET
    this->mRouteTable.addEndpoint < WebMinerAPI::BlockListHandler >         ( "/blocks/?" );                            // GET
    this->mRouteTable.addEndpoint < WebMinerAPI::KeyDetailsHandler >        ( "/keys/:keyHash/?" );                     // GET
    this->mRouteTable.addEndpoint < WebMinerAPI::MinerListHandler >         ( "/miners/?" );                            // GET
    this->mRouteTable.addEndpoint < WebMinerAPI::SchemaListHandler >        ( "/schemas/?" );                           // GET
    this->mRouteTable.addEndpoint < WebMinerAPI::ExtendChainHandler >       ( "/test/extendChain/?" );                  // POST
    this->mRouteTable.addEndpoint < WebMinerAPI::TestKeyIDHandler >         ( "/test/keyid/?" );                        // POST
    this->mRouteTable.addEndpoint < WebMinerAPI::TransactionHandler >       ( "/transactions/?" );                      // POST
    this->mRouteTable.setDefault < WebMinerAPI::DefaultHandler >            ();
}

//----------------------------------------------------------------//
HTTPRequestHandlerFactory::~HTTPRequestHandlerFactory () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
Poco::Net::HTTPRequestHandler* HTTPRequestHandlerFactory::createRequestHandler ( const Poco::Net::HTTPServerRequest& request ) {
    
    return this->mRouteTable.match ( request.getURI ());
}

} // WebMinerAPI
} // namespace Volition
