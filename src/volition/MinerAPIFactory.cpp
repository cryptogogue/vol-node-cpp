// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/HTTP.h>

#include <volition/web-miner-api/AccountDetailsHandler.h>
#include <volition/web-miner-api/AccountKeyListHandler.h>
#include <volition/web-miner-api/AccountLogHandler.h>
#include <volition/web-miner-api/AssetDetailsHandler.h>
#include <volition/web-miner-api/BlockDetailsHandler.h>
#include <volition/web-miner-api/BlockListHandler.h>
#include <volition/web-miner-api/ConsensusBlockDetailsHandler.h>
#include <volition/web-miner-api/ConsensusBlockHeaderListHandler.h>
#include <volition/web-miner-api/ConsensusPeekHandler.h>
#include <volition/web-miner-api/ControlCommandHandler.h>
#include <volition/web-miner-api/DebugHTTPEchoHandler.h>
#include <volition/web-miner-api/DebugKeyGenHandler.h>
#include <volition/web-miner-api/DefaultHandler.h>
#include <volition/web-miner-api/InventoryAssetsHandler.h>
#include <volition/web-miner-api/InventoryHandler.h>
#include <volition/web-miner-api/InventoryLogHandler.h>
#include <volition/web-miner-api/KeyAccountDetailsHandler.h>
#include <volition/web-miner-api/KeyDetailsHandler.h>
#include <volition/web-miner-api/MinerListHandler.h>
#include <volition/web-miner-api/NodeDetailsHandler.h>
#include <volition/web-miner-api/OfferDetailsHandler.h>
#include <volition/web-miner-api/OfferListHandler.h>
#include <volition/web-miner-api/ResetChainHandler.h>
#include <volition/web-miner-api/SchemaHandler.h>
#include <volition/web-miner-api/TermsOfServiceHandler.h>
#include <volition/web-miner-api/TestExceptions.h>
#include <volition/web-miner-api/TestKeyIDHandler.h>
#include <volition/web-miner-api/TestSignatureHandler.h>
#include <volition/web-miner-api/TransactionHandler.h>
#include <volition/web-miner-api/TransactionQueueHandler.h>
#include <volition/web-miner-api/VisageHandler.h>

#include <volition/MinerAPIFactory.h>

namespace Volition {

//================================================================//
// MinerAPIFactory
//================================================================//

//----------------------------------------------------------------//
void MinerAPIFactory::initializeRoutes () {

    cc8* prefix = this->mWithPrefix ? "/:minerID" : "";

    // TODO: stardardize endpoint path format

    this->mRouteTable.addEndpoint < WebMinerAPI::TermsOfServiceHandler >                ( HTTP::GET,        Format::write ( "%s/tos/?", prefix ));

    this->mRouteTable.addEndpoint < WebMinerAPI::AccountDetailsHandler >                ( HTTP::GET,        Format::write ( "%s/accounts/:accountName/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::AccountLogHandler >                    ( HTTP::GET,        Format::write ( "%s/accounts/:accountName/log/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::InventoryHandler >                     ( HTTP::GET,        Format::write ( "%s/accounts/:accountName/inventory/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::InventoryAssetsHandler >               ( HTTP::GET,        Format::write ( "%s/accounts/:accountName/inventory/assets/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::InventoryLogHandler >                  ( HTTP::GET,        Format::write ( "%s/accounts/:accountName/inventory/log/:nonce/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::AccountKeyListHandler >                ( HTTP::GET,        Format::write ( "%s/accounts/:accountName/keys/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::TransactionHandler >                   ( HTTP::GET_PUT,    Format::write ( "%s/accounts/:accountName/transactions/:uuid/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::TransactionQueueHandler >              ( HTTP::GET,        Format::write ( "%s/accounts/:accountName/transactions/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::AssetDetailsHandler >                  ( HTTP::GET,        Format::write ( "%s/assets/:assetIndexOrID/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::BlockDetailsHandler >                  ( HTTP::GET,        Format::write ( "%s/blocks/:blockID/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::BlockListHandler >                     ( HTTP::GET,        Format::write ( "%s/blocks/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::OfferDetailsHandler >                  ( HTTP::GET,        Format::write ( "%s/offers/:assetID/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::OfferListHandler >                     ( HTTP::GET,        Format::write ( "%s/offers/?", prefix ));

    this->mRouteTable.addEndpoint < WebMinerAPI::ConsensusBlockDetailsHandler >         ( HTTP::GET,        Format::write ( "%s/consensus/blocks/:hash/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::ConsensusBlockHeaderListHandler >      ( HTTP::GET,        Format::write ( "%s/consensus/headers/?", prefix )); // TODO: better regex for query params
    this->mRouteTable.addEndpoint < WebMinerAPI::ConsensusPeekHandler >                 ( HTTP::GET,        Format::write ( "%s/consensus/peek/?", prefix ));
    
    this->mRouteTable.addEndpoint < WebMinerAPI::ControlCommandHandler >                ( HTTP::POST,       Format::write ( "%s/control/?", prefix ));
    
    this->mRouteTable.addEndpoint < WebMinerAPI::KeyAccountDetailsHandler >             ( HTTP::GET,        Format::write ( "%s/keys/:keyHash/account/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::KeyDetailsHandler >                    ( HTTP::GET,        Format::write ( "%s/keys/:keyHash/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::MinerListHandler >                     ( HTTP::GET,        Format::write ( "%s/miners/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::NodeDetailsHandler >                   ( HTTP::GET,        Format::write ( "%s/node/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::SchemaHandler >                        ( HTTP::GET,        Format::write ( "%s/schema/?", prefix ));

    this->mRouteTable.addEndpoint < WebMinerAPI::VisageHandler >                        ( HTTP::GET,        Format::write ( "%s/visage/?", prefix ));

    this->mRouteTable.addEndpoint < WebMinerAPI::TestExceptions >                       ( HTTP::GET,        Format::write ( "%s/test/exceptions/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::TestKeyIDHandler >                     ( HTTP::POST,       Format::write ( "%s/test/keyid/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::TestSignatureHandler >                 ( HTTP::POST,       Format::write ( "%s/test/signature/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::DebugHTTPEchoHandler >                 ( HTTP::ALL,        Format::write ( "%s/debug/echo/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::DebugKeyGenHandler >                   ( HTTP::GET,        Format::write ( "%s/debug/keygen/:type/?", prefix ));
    this->mRouteTable.addEndpoint < WebMinerAPI::ResetChainHandler >                    ( HTTP::DELETE,     Format::write ( "%s/?", prefix ));
    
    this->mRouteTable.setDefault < WebMinerAPI::DefaultHandler >                        ();
}

//----------------------------------------------------------------//
MinerAPIFactory::MinerAPIFactory ( shared_ptr < Miner > minerActivity ) :
    mWithPrefix ( false ) {
    
    this->mMiner = minerActivity;
    this->initializeRoutes ();
}

//----------------------------------------------------------------//
MinerAPIFactory::MinerAPIFactory ( const vector < shared_ptr < Miner >>& webMiners ) :
    mWithPrefix ( true ) {
    
    for ( size_t i = 0; i < webMiners.size (); ++i ) {
        shared_ptr < Miner > miner = webMiners [ i ];
        this->mMiners [ miner->getMinerID ()] = miner;
    }
    this->initializeRoutes ();
}

//----------------------------------------------------------------//
MinerAPIFactory::~MinerAPIFactory () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
Poco::Net::HTTPRequestHandler* MinerAPIFactory::createRequestHandler ( const Poco::Net::HTTPServerRequest& request ) {
    
    unique_ptr < AbstractMinerAPIRequestHandler > handler = this->mRouteTable.match ( request );
    
    if ( this->mWithPrefix ) {
        string minerID = handler->getMatchString ( "minerID" );
        map < string, shared_ptr < Miner >>::iterator webMinerIt = this->mMiners.find ( minerID );
        if ( webMinerIt != this->mMiners.end ()) {
            handler->initialize ( webMinerIt->second );
        }
    }
    else {
        handler->initialize ( this->mMiner );
    }
    
    return handler.release ();
}

} // namespace Volition
