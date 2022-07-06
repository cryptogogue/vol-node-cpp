// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONMAKERQUEUE_H
#define VOLITION_TRANSACTIONMAKERQUEUE_H

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
class TransactionEnvelope;

//================================================================//
// TransactionMakerQueue
//================================================================//
class TransactionMakerQueue {
public:

    enum Status {
        STATUS_OK,
        BLOCKED_ON_ERROR,
        BLOCKED_ON_IGNORE,
    };

    typedef map < u64, shared_ptr < const TransactionEnvelope >>::iterator              TransactionQueueIt;
    typedef map < u64, shared_ptr < const TransactionEnvelope >>::const_iterator        TransactionQueueConstIt;

    typedef map < u64, shared_ptr < const TransactionEnvelope >>                        Queue;

    typedef map < string, shared_ptr < const TransactionEnvelope >>::const_iterator     TransactionLookupConstIt;
    typedef map < string, shared_ptr < const TransactionEnvelope >>                     Lookup;

protected:

    friend class TransactionQueue;
    
    Queue                               mQueue;
    Lookup                              mLookup;

    Status                              mQueueStatus;
    TransactionStatus                   mTransactionStatus;

    string                              mAccountName;
    u64                                 mCount;

public:

    GET ( const Queue&,     Queue,          mQueue )
    IS ( Makerless,         mAccountName,   "" )

    //----------------------------------------------------------------//
    TransactionResult                   checkTransactionOrder           ( u64 nonce ) const;
    shared_ptr < const TransactionEnvelope >    getTransaction                  ( u64 positionOrNonce ) const;
    shared_ptr < const TransactionEnvelope >    getTransaction                  ( string uuid ) const;
    bool                                hasTransactions                 () const;
    void                                ignoreTransaction               ( string message, string uuid );
    bool                                isBlocked                       () const;
                                        TransactionMakerQueue           ();
    void                                pushTransaction                 ( shared_ptr < const TransactionEnvelope > transaction );
    void                                prune                           ( u64 nonce );
    void                                prune                           ( const AbstractLedger& chain );
    void                                setTransactionResult            ( TransactionResult result );
};

} // namespace Volition
#endif
