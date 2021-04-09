// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LOCKINGMINERAPIREQUESTHANDLER_H
#define VOLITION_LOCKINGMINERAPIREQUESTHANDLER_H

#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/HTTP.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>

namespace Volition {

class AbstractConsensusInspector;
class Ledger;
class Miner;

//================================================================//
// BlockingMinerAPIRequestHandler
//================================================================//
class BlockingMinerAPIRequestHandler :
    public AbstractMinerAPIRequestHandler {
protected:

    friend class MinerAPIFactory;
    
    shared_ptr < Miner >    mWebMiner;
    
    //----------------------------------------------------------------//
    virtual HTTPStatus      BlockingMinerAPIRequestHandler_handleRequest        ( HTTP::Method method, AbstractLedger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const = 0;

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
    
        ScopedMinerLock scopedLock ( this->mWebMiner );
    
        u64 totalBlocks = this->mWebMiner->getLedger ().countBlocks ();
        u64 height = this->optQuery ( "at", totalBlocks );
        Ledger ledger ( this->mWebMiner->getLedgerAtBlock ( height ));
        
        return this->BlockingMinerAPIRequestHandler_handleRequest ( method, ledger, jsonIn, jsonOut );
    }
    
     //----------------------------------------------------------------//
    void AbstractMinerAPIRequestHandler_initialize ( shared_ptr < Miner > miner ) override {
    
        this->mWebMiner = miner;
    }

public:

    //----------------------------------------------------------------//
    BlockingMinerAPIRequestHandler () {
    }
    
    //----------------------------------------------------------------//
    ~BlockingMinerAPIRequestHandler () {
    }
};

} // namespace Volition
#endif
