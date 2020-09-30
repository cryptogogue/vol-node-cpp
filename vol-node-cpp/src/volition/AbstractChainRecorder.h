// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTCHAINRECORDER_H
#define VOLITION_ABSTRACTCHAINRECORDER_H

#include <volition/common.h>
#include <volition/CryptoKey.h>
#include <volition/Chain.h>
#include <volition/ChainMetadata.h>
#include <volition/Ledger.h>
#include <volition/MinerBase.h>
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
    static void pushBlock ( MinerBase& miner, shared_ptr < Block > block ) {
    
        miner.pushBlock ( block );
    }

    //----------------------------------------------------------------//
    virtual void            AbstractChainRecorder_loadChain         ( MinerBase& miner ) const = 0;
    virtual void            AbstractChainRecorder_reset             () = 0;
    virtual void            AbstractChainRecorder_saveChain         ( const MinerBase& miner ) = 0;

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
    void loadChain ( MinerBase& miner ) const {
        this->AbstractChainRecorder_loadChain ( miner );
    }
    
    //----------------------------------------------------------------//
    void reset () {
        this->AbstractChainRecorder_reset ();
    }
    
    //----------------------------------------------------------------//
    void saveChain ( const MinerBase& miner ) {
        this->AbstractChainRecorder_saveChain ( miner );
    }
};

} // namespace Volition
#endif
