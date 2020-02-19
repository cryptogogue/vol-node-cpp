// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONQUEUE_H
#define VOLITION_TRANSACTIONQUEUE_H

#include <volition/common.h>
#include <volition/CryptoKey.h>
#include <volition/Chain.h>
#include <volition/ChainMetadata.h>
#include <volition/Ledger.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/Singleton.h>
#include <volition/Transaction.h>

namespace Volition {

class AbstractChainRecorder;
class AbstractHashable;
class Block;

//================================================================//
// MakerQueue
//================================================================//
class MakerQueue {
protected:

    friend class TransactionQueue;

    typedef map < u64, shared_ptr < const Transaction >>::iterator TransactionIt;
    typedef map < u64, shared_ptr < const Transaction >>::const_iterator TransactionConstIt;
    typedef map < u64, shared_ptr < const Transaction >>  Queue;
    
    Queue                   mQueue;
    TransactionResult       mLastResult;

public:

    //----------------------------------------------------------------//
    shared_ptr < const Transaction >    getTransaction          ( u64 nonce ) const;
    bool                                hasError                () const;
    bool                                hasTransaction          ( u64 nonce ) const;
    bool                                hasTransactions         () const;
                                        MakerQueue              ();
    bool                                pushTransaction         ( shared_ptr < const Transaction > transaction );
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
    
    //----------------------------------------------------------------//
    const MakerQueue*       getMakerQueueOrNull     ( string accountName ) const;
    void                    reset                   ();
    
public:

    //----------------------------------------------------------------//
    void                    fillBlock               ( Chain& chain, Block& block );
    TransactionResult       getLastResult           ( string accountName ) const;
    string                  getTransactionNote      ( string accountName, u64 nonce ) const;
    bool                    hasError                ( string accountName );
    void                    pruneTransactions       ( const Chain& chain );
    bool                    pushTransaction         ( shared_ptr < const Transaction > transaction );
                            TransactionQueue        ();
    virtual                 ~TransactionQueue       ();
};

} // namespace Volition
#endif
