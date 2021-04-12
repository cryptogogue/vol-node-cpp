// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SEMIBLOCKINGMINERAPIREQUESTHANDLER_H
#define VOLITION_SEMIBLOCKINGMINERAPIREQUESTHANDLER_H

#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/HTTP.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>

namespace Volition {

class AbstractConsensusInspector;
class Ledger;
class Miner;

//================================================================//
// SemiBlockingMinerAPIRequestHandler
//================================================================//
class SemiBlockingMinerAPIRequestHandler :
    public AbstractMinerAPIRequestHandler {
private:

    friend class MinerAPIFactory;
    
    shared_ptr < Miner >    mWebMiner;

protected:

    mutable MinerSnapshot   mSnapshot;
    mutable MinerStatus     mStatus;

    //----------------------------------------------------------------//
    virtual HTTPStatus      SemiBlockingMinerAPIRequestHandler_handleRequest    ( HTTP::Method method, AbstractLedger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const = 0;

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
    
        this->mWebMiner->getSnapshot ( this->mSnapshot, this->mStatus );
    
        ScopedMinerLedgerLock scopedLock ( this->mWebMiner );

        LockedLedger& lockedLedger = scopedLock.getImmutableLedger ();
        LockedLedgerIterator ledger ( lockedLedger );

        u64 totalBlocks = ledger.countBlocks ();
        u64 height = this->optQuery ( "at", totalBlocks );
        ledger.revert ( height );
        
        return this->SemiBlockingMinerAPIRequestHandler_handleRequest ( method, ledger, jsonIn, jsonOut );
    }
    
     //----------------------------------------------------------------//
    void AbstractMinerAPIRequestHandler_initialize ( shared_ptr < Miner > miner ) override {
    
        this->mWebMiner = miner;
    }

public:

    //----------------------------------------------------------------//
    SemiBlockingMinerAPIRequestHandler () {
    }
    
    //----------------------------------------------------------------//
    ~SemiBlockingMinerAPIRequestHandler () {
    }
};

} // namespace Volition
#endif
