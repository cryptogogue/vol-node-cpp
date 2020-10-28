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
protected:

    friend class TransactionQueue;

    typedef map < u64, shared_ptr < const Transaction >>::iterator              TransactionQueueIt;
    typedef map < u64, shared_ptr < const Transaction >>::const_iterator        TransactionQueueConstIt;
    typedef map < u64, shared_ptr < const Transaction >>                        Queue;
    
    typedef map < string, shared_ptr < const Transaction >>::const_iterator     TransactionLookupConstIt;
    typedef map < string, shared_ptr < const Transaction >>                     Lookup;
    
    Queue                               mQueue;
    Lookup                              mLookup;
    TransactionResult                   mLastResult;

public:

    //----------------------------------------------------------------//
    shared_ptr < const Transaction >    getTransaction          ( string uuid ) const;
    bool                                hasError                () const;
    bool                                hasTransaction          ( u64 nonce ) const;
    bool                                hasTransactions         () const;
                                        MakerQueue              ();
    shared_ptr < const Transaction >    nextTransaction         ( u64 nonce ) const;
    void                                pushTransaction         ( shared_ptr < const Transaction > transaction );
    void                                prune                   ( u64 nonce );
    void                                setError                ( TransactionResult error );
};

//================================================================//
// TransactionQueue
//================================================================//
class TransactionQueue {
protected:

    typedef map < string, MakerQueue >::iterator MakerQueueIt;
    typedef map < string, MakerQueue >::const_iterator MakerQueueConstIt;

    map < string, MakerQueue > mDatabase;
    list < shared_ptr < const Transaction >> mIncoming;
    
    //----------------------------------------------------------------//
    void                    acceptTransaction       ( shared_ptr < const Transaction > transaction );
    const MakerQueue*       getMakerQueueOrNull     ( string accountName ) const;
    void                    processTransactions     ();
    void                    reset                   ();
    
public:

    //----------------------------------------------------------------//
    void                    fillBlock               ( Ledger& chain, Block& block, Block::VerificationPolicy policy );
    TransactionResult       getLastResult           ( string accountName ) const;
    bool                    hasError                ( string accountName );
    bool                    hasTransaction          ( string accountName, string uuid ) const;
    void                    pruneTransactions       ( const Ledger& chain );
    void                    pushTransaction         ( shared_ptr < const Transaction > transaction );
    void                    setError                ( shared_ptr < const Transaction > transaction, TransactionResult error );
                            TransactionQueue        ();
    virtual                 ~TransactionQueue       ();
};

} // namespace Volition
#endif
