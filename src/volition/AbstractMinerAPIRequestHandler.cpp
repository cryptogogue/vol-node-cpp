// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/HTTP.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>
#include <volition/AbstractMinerAPIRequestHandler.h>

namespace Volition {

//================================================================//
// AbstractAPIRequestHandlerWithMiner
//================================================================//
    
//----------------------------------------------------------------//
AbstractAPIRequestHandlerWithMiner::AbstractAPIRequestHandlerWithMiner () {
}

//----------------------------------------------------------------//
AbstractAPIRequestHandlerWithMiner::~AbstractAPIRequestHandlerWithMiner () {
}

//----------------------------------------------------------------//
void AbstractAPIRequestHandlerWithMiner::initialize ( shared_ptr < Miner > miner ) {

    this->AbstractAPIRequestHandlerWithMiner_initialize ( miner );
}

//================================================================//
// AbstractMinerAPIRequestHandler
//================================================================//
    
//----------------------------------------------------------------//
AbstractMinerAPIRequestHandler::AbstractMinerAPIRequestHandler () {
}

//----------------------------------------------------------------//
AbstractMinerAPIRequestHandler::~AbstractMinerAPIRequestHandler () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
AbstractAPIRequestHandler::HTTPStatus AbstractMinerAPIRequestHandler::AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const {

    ScopedMinerLock scopedLock ( this->mWebMiner );
    
    u64 totalBlocks = this->mWebMiner->getLedger ().countBlocks ();
    u64 height = this->optQuery ( "at", totalBlocks );
    Ledger ledger ( this->mWebMiner->getLedgerAtBlock ( height ));
    
    return this->AbstractMinerAPIRequestHandler_handleRequest ( method, ledger, jsonIn, jsonOut );
}

//----------------------------------------------------------------//
void AbstractMinerAPIRequestHandler::AbstractAPIRequestHandlerWithMiner_initialize ( shared_ptr < Miner > miner ) {

    this->mWebMiner = miner;
}

//================================================================//
// AbstractConsensusInspectorAPIRequestHandler
//================================================================//
    
//----------------------------------------------------------------//
AbstractConsensusInspectorAPIRequestHandler::AbstractConsensusInspectorAPIRequestHandler () {
}

//----------------------------------------------------------------//
AbstractConsensusInspectorAPIRequestHandler::~AbstractConsensusInspectorAPIRequestHandler () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
AbstractAPIRequestHandler::HTTPStatus AbstractConsensusInspectorAPIRequestHandler::AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const {

    assert ( this->mInspector );
    return this->AbstractConsensusInspectorAPIRequestHandler_handleRequest ( method, *this->mInspector, jsonIn, jsonOut );
}

//----------------------------------------------------------------//
void AbstractConsensusInspectorAPIRequestHandler::AbstractAPIRequestHandlerWithMiner_initialize ( shared_ptr < Miner > miner ) {

    this->mInspector = miner->createInspector ();
}

} // namespace Volition
