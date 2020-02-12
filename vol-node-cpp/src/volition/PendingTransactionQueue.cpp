// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractChainRecorder.h>
#include <volition/Block.h>
#include <volition/PendingTransactionQueue.h>
#include <volition/TheContext.h>
#include <volition/SyncChainTask.h>

namespace Volition {

//================================================================//
// PendingTransactionQueue
//================================================================//

//----------------------------------------------------------------//
void PendingTransactionQueue::fillBlock ( Chain& chain, Block& block ) {

    Ledger ledger;
    ledger.takeSnapshot ( chain );
    SchemaHandle schemaHandle ( ledger );

    // TODO: this is the naive implementation; there's so much more to do here.
    // for example, the current implementation naively re-applies previously recorded
    // transactions with no consideration of the nonce.

    map < string, MakerQueue >::iterator makerQueueIt = this->mDatabase.begin ();
    for ( ; makerQueueIt != this->mDatabase.end (); ++makerQueueIt ) {
        MakerQueue& makerQueue = makerQueueIt->second;
    
        map < u64, shared_ptr < Transaction >>::iterator transactionIt = makerQueue.begin ();
        for ( ; transactionIt != makerQueue.end (); ++transactionIt ) {
        
            shared_ptr < Transaction > transaction = transactionIt->second;
            
            // push a version in case the transaction fails
            ledger.pushVersion ();
            
            // TODO: don't need to fully apply; should just check maker's nonce and sig
            if ( transaction->apply ( ledger, schemaHandle )) {
                block.pushTransaction ( transaction );
            }
            else {
                // didn't like that transaction; revert and continue
                ledger.popVersion ();
            }
        }
    }
}

//----------------------------------------------------------------//
PendingTransactionQueue::PendingTransactionQueue () {
}

//----------------------------------------------------------------//
PendingTransactionQueue::~PendingTransactionQueue () {
}

//----------------------------------------------------------------//
bool PendingTransactionQueue::pushTransaction ( shared_ptr < Transaction > transaction ) {

    const TransactionMaker* maker = transaction->getMaker ();
    if ( !maker ) return false;

    MakerQueue& queue = this->mDatabase [ maker->getAccountName ()];
    queue [ maker->getNonce ()] = transaction;
    return true;
}

//----------------------------------------------------------------//
void PendingTransactionQueue::reset () {

    this->mDatabase.clear ();
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
