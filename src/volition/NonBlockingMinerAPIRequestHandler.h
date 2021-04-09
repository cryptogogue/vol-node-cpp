// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_NONBLOCKINGMINERAPIREQUESTHANDLER_H
#define VOLITION_NONBLOCKINGMINERAPIREQUESTHANDLER_H

#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/HTTP.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>

namespace Volition {

class AbstractConsensusInspector;
class Ledger;
class Miner;

//================================================================//
// NonBlockingMinerAPIRequestHandler
//================================================================//
class NonBlockingMinerAPIRequestHandler :
    public AbstractMinerAPIRequestHandler {
private:

    shared_ptr < Miner >    mWebMiner;

protected:

    friend class MinerAPIFactory;
    
    mutable MinerSnapshot   mSnapshot;
    mutable MinerStatus     mStatus;
    
    //----------------------------------------------------------------//
    virtual HTTPStatus      NonBlockingMinerAPIRequestHandler_handleRequest     ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const = 0;
    
    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        
        this->mWebMiner->getSnapshot ( this->mSnapshot, this->mStatus );

        return this->NonBlockingMinerAPIRequestHandler_handleRequest ( method, jsonIn, jsonOut );
    }
    
    //----------------------------------------------------------------//
    void AbstractMinerAPIRequestHandler_initialize ( shared_ptr < Miner > miner ) override {
    
        this->mWebMiner = miner;
    }

public:

    //----------------------------------------------------------------//
    NonBlockingMinerAPIRequestHandler () {
    }
    
    //----------------------------------------------------------------//
    ~NonBlockingMinerAPIRequestHandler () {
    }
};

} // namespace Volition
#endif
