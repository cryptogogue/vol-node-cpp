// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/http.h>

#include <volition/web-miner-api/AccountDetailsHandler.h>
#include <volition/web-miner-api/AccountKeyListHandler.h>
#include <volition/web-miner-api/AccountTransactionHandler.h>
#include <volition/web-miner-api/AssetDetailsHandler.h>
#include <volition/web-miner-api/BlockDetailsHandler.h>
#include <volition/web-miner-api/BlockListHandler.h>
#include <volition/web-miner-api/DefaultHandler.h>
#include <volition/web-miner-api/ExtendChainHandler.h>
#include <volition/web-miner-api/InventoryAssetsHandler.h>
#include <volition/web-miner-api/InventoryHandler.h>
#include <volition/web-miner-api/InventoryLogHandler.h>
#include <volition/web-miner-api/KeyAccountDetailsHandler.h>
#include <volition/web-miner-api/KeyDetailsHandler.h>
#include <volition/web-miner-api/MinerListHandler.h>
#include <volition/web-miner-api/ResetChainHandler.h>
#include <volition/web-miner-api/SchemaHandler.h>
#include <volition/web-miner-api/TestKeyIDHandler.h>
#include <volition/web-miner-api/TestSignatureHandler.h>

#include <volition/WebMinerAPIFactory.h>

namespace Volition {

//================================================================//
// WebMinerAPIFactory
//================================================================//

//----------------------------------------------------------------//
void WebMinerAPIFactory::initializeRoutes () {

    cc8* prefix = this->mWithPrefix ? "/:minerID" : "";

    this->mRouteTable.addEndpoint < WebMinerAPI::AccountDetailsHandler >        ( HTTP::GET,        Format::write ( "%s/accounts/:accountName/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::InventoryHandler >             ( HTTP::GET,        Format::write ( "%s/accounts/:accountName/inventory/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::InventoryAssetsHandler >       ( HTTP::GET,        Format::write ( "%s/accounts/:accountName/inventory/assets/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::InventoryLogHandler >          ( HTTP::GET,        Format::write ( "%s/accounts/:accountName/inventory/log/:nonce/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::AccountKeyListHandler >        ( HTTP::GET,        Format::write ( "%s/accounts/:accountName/keys/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::AccountTransactionHandler >    ( HTTP::GET_PUT,    Format::write ( "%s/accounts/:accountName/transactions/:uuid/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::AssetDetailsHandler >          ( HTTP::GET,        Format::write ( "%s/assets/:assetIndexOrID/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::BlockDetailsHandler >          ( HTTP::GET,        Format::write ( "%s/blocks/:blockID/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::BlockListHandler >             ( HTTP::GET,        Format::write ( "%s/blocks/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::KeyAccountDetailsHandler >     ( HTTP::GET,        Format::write ( "%s/keys/:keyHash/account/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::KeyDetailsHandler >            ( HTTP::GET,        Format::write ( "%s/keys/:keyHash/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::MinerListHandler >             ( HTTP::GET,        Format::write ( "%s/miners/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::SchemaHandler >                ( HTTP::GET,        Format::write ( "%s/schema/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::ExtendChainHandler >           ( HTTP::POST,       Format::write ( "%s/test/extendChain/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::TestKeyIDHandler >             ( HTTP::POST,       Format::write ( "%s/test/keyid/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::TestSignatureHandler >         ( HTTP::POST,       Format::write ( "%s/test/signature/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::ResetChainHandler >            ( HTTP::DELETE,     Format::write ( "%s/?", prefix ));
    this->mRouteTable.setDefault < WebMinerAPI::DefaultHandler >                ();
}

//----------------------------------------------------------------//
WebMinerAPIFactory::WebMinerAPIFactory ( shared_ptr < WebMiner > webMiner ) :
    mWithPrefix ( false ) {
    
    this->mWebMiner = webMiner;
    this->initializeRoutes ();
}

//----------------------------------------------------------------//
WebMinerAPIFactory::WebMinerAPIFactory ( vector < shared_ptr < WebMiner >> webMiners ) :
    mWithPrefix ( true ) {
    
    for ( size_t i = 0; i < webMiners.size (); ++i ) {
        shared_ptr < WebMiner > miner = webMiners [ i ];
        this->mWebMiners [ miner->getMinerID ()] = miner;
    }
    this->initializeRoutes ();
}

//----------------------------------------------------------------//
WebMinerAPIFactory::~WebMinerAPIFactory () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
Poco::Net::HTTPRequestHandler* WebMinerAPIFactory::createRequestHandler ( const Poco::Net::HTTPServerRequest& request ) {
    
    unique_ptr < WebMinerAPIRequestHandler > handler = this->mRouteTable.match ( request );
    
    if ( this->mWithPrefix ) {
        string minerID = handler->getMatchString ( "minerID" );
        map < string, shared_ptr < WebMiner >>::iterator webMinerIt = this->mWebMiners.find ( minerID );
        if ( webMinerIt != this->mWebMiners.end ()) {
            handler->mWebMiner = webMinerIt->second;
        }
    }
    else {
        handler->mWebMiner = this->mWebMiner;
    }
    return handler.release ();
}

} // namespace Volition
