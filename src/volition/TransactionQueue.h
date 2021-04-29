// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONQUEUE_H
#define VOLITION_TRANSACTIONQUEUE_H

#include <volition/common.h>
#include <volition/CryptoKey.h>
#include <volition/Ledger.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/Singleton.h>
#include <volition/TransactionResult.h>
#include <volition/TransactionStatus.h>

namespace Volition {

class AbstractChainRecorder;
class AbstractHashable;
class Block;
class Miner;
class Transaction;

//================================================================//
// MakerQueue
//================================================================//
class MakerQueue {
public:

    enum Status {
        STATUS_OK,
        BLOCKED_ON_ERROR,
        BLOCKED_ON_IGNORE,
    };

    typedef map < u64, shared_ptr < const Transaction >>::iterator              TransactionQueueIt;
    typedef map < u64, shared_ptr < const Transaction >>::const_iterator        TransactionQueueConstIt;

    typedef map < u64, shared_ptr < const Transaction >>                        Queue;

    typedef map < string, shared_ptr < const Transaction >>::const_iterator     TransactionLookupConstIt;
    typedef map < string, shared_ptr < const Transaction >>                     Lookup;

protected:

    friend class TransactionQueue;
    
    Queue                               mQueue;
    Lookup                              mLookup;

    Status                              mQueueStatus;
    TransactionStatus                   mTransactionStatus;

public:

    GET ( const Queue&,     Queue,      mQueue )

    //----------------------------------------------------------------//
    TransactionResult                   checkForPendingTransactions     ( u64 nonce ) const;
    shared_ptr < const Transaction >    getTransaction                  ( string uuid ) const;
    bool                                hasTransaction                  ( u64 nonce ) const;
    bool                                hasTransactions                 () const;
    void                                ignoreTransaction               ( string message, string uuid );
    bool                                isBlocked                       () const;
                                        MakerQueue                      ();
    shared_ptr < const Transaction >    nextTransaction                 ( u64 nonce ) const;
    void                                pushTransaction                 ( shared_ptr < const Transaction > transaction );
    void                                prune                           ( u64 nonce );
    void                                setTransactionResult            ( TransactionResult result );
};

//================================================================//
// TransactionQueue
//================================================================//
class TransactionQueue {
protected:

    typedef map < string, MakerQueue >::iterator MakerQueueIt;
    typedef map < string, MakerQueue >::const_iterator MakerQueueConstIt;

    map < string, MakerQueue > mDatabase;
    
    //----------------------------------------------------------------//
    void                    acceptTransaction       ( shared_ptr < const Transaction > transaction );
    
public:

    //----------------------------------------------------------------//
    void                    fillBlock               ( Ledger& chain, Block& block, Block::VerificationPolicy policy, u64 minimumGratuity = 0 );
    TransactionStatus       getLastStatus           ( string accountName ) const;
    const MakerQueue*       getMakerQueueOrNull     ( string accountName ) const;
    TransactionStatus       getTransactionStatus    ( const AbstractLedger& ledger, string accountName, string uuid ) const;
    bool                    hasTransaction          ( string accountName, string uuid ) const;
    bool                    isBlocked               ( string accountName ) const;
    void                    pruneTransactions       ( const AbstractLedger& chain );
    void                    pushTransaction         ( shared_ptr < const Transaction > transaction );
    void                    reset                   ();
                            TransactionQueue        ();
    virtual                 ~TransactionQueue       ();
};

} // namespace Volition
#endif
