// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTMINERAPIREQUESTHANDLER_H
#define VOLITION_ABSTRACTMINERAPIREQUESTHANDLER_H

#include <volition/AbstractAPIRequestHandler.h>
#include <volition/HTTP.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>
#include <volition/MinerLocks.h>

namespace Volition {

class Ledger;
class Miner;

//================================================================//
// AbstractMinerAPIRequestHandler
//================================================================//
class AbstractMinerAPIRequestHandler :
    public AbstractAPIRequestHandler {
protected:

    friend class MinerAPIFactory;
    
    shared_ptr < Miner >    mMiner;
    
    //----------------------------------------------------------------//
    virtual HTTPStatus      AbstractMinerAPIRequestHandler_handleRequest        ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const = 0;
    
    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        return this->AbstractMinerAPIRequestHandler_handleRequest ( method, this->mMiner, jsonIn, jsonOut );
    }

public:

    //----------------------------------------------------------------//
    AbstractMinerAPIRequestHandler () {
    }
    
     //----------------------------------------------------------------//
    ~AbstractMinerAPIRequestHandler () {
    }
    
     //----------------------------------------------------------------//
    void initialize ( shared_ptr < Miner > miner ) {
    
        this->mMiner = miner;
    }
};

////----------------------------------------------------------------//
//HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
//
//    this->mWebMiner->getSnapshot ( this->mSnapshot, this->mStatus );
//
//    ScopedSharedMinerLedgerLock scopedLock ( this->mWebMiner );
//    scopedLock.seek ( this->optQuery ( "at", scopedLock.countBlocks ()));
//    
//    return this->AbstractMinerAPIRequestHandler_handleRequest ( method, scopedLock, jsonIn, jsonOut );
//}

} // namespace Volition
#endif
