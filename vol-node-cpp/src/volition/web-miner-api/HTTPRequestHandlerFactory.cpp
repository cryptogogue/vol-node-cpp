// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/web-miner-api/AccountDetailsHandler.h>
#include <volition/web-miner-api/AccountKeyListHandler.h>
#include <volition/web-miner-api/AccountTransactionHandler.h>
#include <volition/web-miner-api/AssetDetailsHandler.h>
#include <volition/web-miner-api/BlockDetailsHandler.h>
#include <volition/web-miner-api/BlockListHandler.h>
#include <volition/web-miner-api/DefaultHandler.h>
#include <volition/web-miner-api/ExtendChainHandler.h>
#include <volition/web-miner-api/HTTPRequestHandlerFactory.h>
#include <volition/web-miner-api/InventoryHandler.h>
#include <volition/web-miner-api/KeyAccountDetailsHandler.h>
#include <volition/web-miner-api/KeyDetailsHandler.h>
#include <volition/web-miner-api/MinerListHandler.h>
#include <volition/web-miner-api/ResetChainHandler.h>
#include <volition/web-miner-api/SchemaHandler.h>
#include <volition/web-miner-api/TestKeyIDHandler.h>
#include <volition/web-miner-api/TestSignatureHandler.h>
#include <volition/web-miner-api/TransactionHandler.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// WebMinerAPI
//================================================================//

//----------------------------------------------------------------//
HTTPRequestHandlerFactory::HTTPRequestHandlerFactory () {

    this->mRouteTable.addEndpoint < WebMinerAPI::AccountDetailsHandler >        ( HTTP::GET,        "/accounts/:accountName/?" );
    this->mRouteTable.addEndpoint < WebMinerAPI::InventoryHandler >             ( HTTP::GET,        "/accounts/:accountName/inventory/?" );
    this->mRouteTable.addEndpoint < WebMinerAPI::AccountKeyListHandler >        ( HTTP::GET,        "/accounts/:accountName/keys/?" );
    this->mRouteTable.addEndpoint < WebMinerAPI::AccountTransactionHandler >    ( HTTP::GET_PUT,    "/accounts/:accountName/transactions/:nonce/?" );
    this->mRouteTable.addEndpoint < WebMinerAPI::AssetDetailsHandler >          ( HTTP::GET,        "/assets/:assetIndexOrID/?" );
    this->mRouteTable.addEndpoint < WebMinerAPI::BlockDetailsHandler >          ( HTTP::GET,        "/blocks/:blockID/?" );
    this->mRouteTable.addEndpoint < WebMinerAPI::BlockListHandler >             ( HTTP::GET,        "/blocks/?" );
    this->mRouteTable.addEndpoint < WebMinerAPI::KeyAccountDetailsHandler >     ( HTTP::GET,        "/keys/:keyHash/account/?" );
    this->mRouteTable.addEndpoint < WebMinerAPI::KeyDetailsHandler >            ( HTTP::GET,        "/keys/:keyHash/?" );
    this->mRouteTable.addEndpoint < WebMinerAPI::MinerListHandler >             ( HTTP::GET,        "/miners/?" );
    this->mRouteTable.addEndpoint < WebMinerAPI::SchemaHandler >                ( HTTP::GET,        "/schema/?" );
    this->mRouteTable.addEndpoint < WebMinerAPI::ExtendChainHandler >           ( HTTP::POST,       "/test/extendChain/?" );
    this->mRouteTable.addEndpoint < WebMinerAPI::TestKeyIDHandler >             ( HTTP::POST,       "/test/keyid/?" );
    this->mRouteTable.addEndpoint < WebMinerAPI::TestSignatureHandler >         ( HTTP::POST,       "/test/signature/?" );
    this->mRouteTable.addEndpoint < WebMinerAPI::ResetChainHandler >            ( HTTP::DELETE,     "/?" );
    this->mRouteTable.setDefault < WebMinerAPI::DefaultHandler >                ();
}

//----------------------------------------------------------------//
HTTPRequestHandlerFactory::~HTTPRequestHandlerFactory () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
Poco::Net::HTTPRequestHandler* HTTPRequestHandlerFactory::createRequestHandler ( const Poco::Net::HTTPServerRequest& request ) {
    
    return this->mRouteTable.match ( request );
}

} // WebMinerAPI
} // namespace Volition
