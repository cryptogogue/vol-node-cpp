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
// TransactionQueue
//================================================================//
class TransactionQueue {
protected:

    typedef map < u64, shared_ptr < Transaction >>  MakerQueue; // nonce: transaction

    map < string, MakerQueue > mDatabase; // accountName: queue
    set < string > mRejected;
    
    //----------------------------------------------------------------//
    void                    reset                   ();
    
public:

    //----------------------------------------------------------------//
    void            fillBlock               ( Chain& chain, Block& block );
    string          getTransactionNote      ( string accountName, u64 nonce ) const;
    bool            isRejected              ( string accountName ) const;
    void            pruneTransactions       ( const Chain& chain );
    bool            pushTransaction         ( shared_ptr < Transaction > transaction );
                    TransactionQueue        ();
    virtual         ~TransactionQueue       ();
};

} // namespace Volition
#endif
