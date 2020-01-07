// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_PENDINGTRANSACTIONQUEUE_H
#define VOLITION_PENDINGTRANSACTIONQUEUE_H

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
// PendingTransactionQueue
//================================================================//
class PendingTransactionQueue {
protected:

    typedef map < u64, shared_ptr < Transaction >>  MakerQueue;

    map < string, MakerQueue > mDatabase;
    
    //----------------------------------------------------------------//
    void                    reset                   ();
    
public:

    //----------------------------------------------------------------//
    void                    fillBlock                   ( Chain& chain, Block& block );
    bool                    pushTransaction             ( shared_ptr < Transaction > transaction );
                            PendingTransactionQueue     ();
    virtual                 ~PendingTransactionQueue    ();
};

} // namespace Volition
#endif
