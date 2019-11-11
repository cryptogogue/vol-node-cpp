// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTCHAINRECORDER_H
#define VOLITION_ABSTRACTCHAINRECORDER_H

#include <volition/common.h>
#include <volition/CryptoKey.h>
#include <volition/Chain.h>
#include <volition/ChainMetadata.h>
#include <volition/Ledger.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/Singleton.h>
#include <volition/Transaction.h>

namespace Volition {

class Miner;

//================================================================//
// AbstractChainRecorder
//================================================================//
class AbstractChainRecorder {
protected:

    //----------------------------------------------------------------//
    virtual void            AbstractChainRecorder_loadChain         ( Miner& miner ) const = 0;
    virtual void            AbstractChainRecorder_saveChain         ( const Miner& miner ) = 0;

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
    void loadChain ( Miner& miner ) const {
        this->AbstractChainRecorder_loadChain ( miner );
    }
    
    //----------------------------------------------------------------//
    void saveChain ( const Miner& miner ) {
        this->AbstractChainRecorder_saveChain ( miner );
    }
};

} // namespace Volition
#endif
