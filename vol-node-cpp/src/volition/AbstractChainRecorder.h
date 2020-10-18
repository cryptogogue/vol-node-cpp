// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTCHAINRECORDER_H
#define VOLITION_ABSTRACTCHAINRECORDER_H

#include <volition/common.h>
#include <volition/CryptoKey.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/Singleton.h>
#include <volition/Transaction.h>

namespace Volition {

//================================================================//
// AbstractChainRecorder
//================================================================//
class AbstractChainRecorder {
protected:

    //----------------------------------------------------------------//
    static void pushBlock ( Miner& miner, shared_ptr < Block > block ) {
    
        miner.pushBlock ( block );
    }

    //----------------------------------------------------------------//
    virtual void            AbstractChainRecorder_loadChain         ( Miner& miner ) = 0;
    virtual void            AbstractChainRecorder_loadConfig        ( MinerConfig& minerConfig ) = 0;
    virtual void            AbstractChainRecorder_reset             () = 0;
    virtual void            AbstractChainRecorder_saveChain         ( const Miner& miner ) = 0;
    virtual void            AbstractChainRecorder_saveConfig        ( const MinerConfig& minerConfig ) = 0;

public:

    enum class SubmissionResponse {
        ACCEPTED = 0,
        RESUBMIT_EARLIER,
    };

    //----------------------------------------------------------------//
    AbstractChainRecorder () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractChainRecorder () {
    }
    
    //----------------------------------------------------------------//
    void loadChain ( Miner& miner ) {
        this->AbstractChainRecorder_loadChain ( miner );
    }
    
    //----------------------------------------------------------------//
    void loadConfig ( MinerConfig& minerConfig ) {
        this->AbstractChainRecorder_loadConfig ( minerConfig );
    }
    
    //----------------------------------------------------------------//
    void reset () {
        this->AbstractChainRecorder_reset ();
    }
    
    //----------------------------------------------------------------//
    void saveChain ( const Miner& miner ) {
        this->AbstractChainRecorder_saveChain ( miner );
    }
    
    //----------------------------------------------------------------//
    void saveConfig ( const MinerConfig& minerConfig ) {
        this->AbstractChainRecorder_saveConfig ( minerConfig );
    }
};

} // namespace Volition
#endif
