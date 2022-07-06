// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONQUEUE_H
#define VOLITION_TRANSACTIONQUEUE_H

#include <volition/common.h>
#include <volition/CryptoKey.h>
#include <volition/Ledger.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/Singleton.h>
#include <volition/TransactionMakerQueue.h>
#include <volition/TransactionResult.h>
#include <volition/TransactionStatus.h>

namespace Volition {

class AbstractChainRecorder;
class AbstractHashable;
class Block;
class Miner;
class TransactionEnvelope;

//================================================================//
// TransactionQueue
//================================================================//
class TransactionQueue {
protected:

    typedef TransactionMakerQueue MakerQueue;
    typedef map < string, TransactionMakerQueue >::iterator MakerQueueIt;
    typedef map < string, TransactionMakerQueue >::const_iterator MakerQueueConstIt;

    map < string, TransactionMakerQueue > mDatabase;
    
    //----------------------------------------------------------------//
    void                    acceptTransaction       ( shared_ptr < const TransactionEnvelope > transaction );
    
public:

    //----------------------------------------------------------------//
    void                    fillBlock               ( Ledger& chain, Block& block, Block::VerificationPolicy policy, u64 minimumGratuity = 0 );
    TransactionStatus       getLastStatus           ( string accountName ) const;
    const MakerQueue*       getMakerQueueOrNull     ( string accountName ) const;
    TransactionStatus       getTransactionStatus    ( const AbstractLedger& ledger, string accountName, string uuid ) const;
    bool                    hasTransaction          ( string accountName, string uuid ) const;
    bool                    isBlocked               ( string accountName ) const;
    void                    pruneTransactions       ( const AbstractLedger& chain );
    void                    pushTransaction         ( shared_ptr < const TransactionEnvelope > transaction );
    void                    reset                   ();
                            TransactionQueue        ();
    virtual                 ~TransactionQueue       ();
};

} // namespace Volition
#endif
