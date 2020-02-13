// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractChainRecorder.h>
#include <volition/Block.h>
#include <volition/TransactionQueue.h>
#include <volition/TheContext.h>
#include <volition/SyncChainTask.h>

namespace Volition {

//================================================================//
// TransactionQueue
//================================================================//

//----------------------------------------------------------------//
void TransactionQueue::fillBlock ( Chain& chain, Block& block ) {

    Ledger ledger;
    ledger.takeSnapshot ( chain );
    SchemaHandle schemaHandle ( ledger );

    // TODO: this is the naive implementation; there's so much more to do here.
    // for example, the current implementation naively re-applies previously recorded
    // transactions with no consideration of the nonce.

    // visit each account
    map < string, MakerQueue >::iterator makerQueueIt = this->mDatabase.begin ();
    for ( ; makerQueueIt != this->mDatabase.end (); ++makerQueueIt ) {
        MakerQueue& makerQueue = makerQueueIt->second;
    
        // TODO: get the account nonce and skip ahead
    
        // visit each transaction (ordered by nonce)
        map < u64, shared_ptr < Transaction >>::iterator transactionIt = makerQueue.begin ();
        for ( ; transactionIt != makerQueue.end (); ++transactionIt ) {
        
            shared_ptr < Transaction > transaction = transactionIt->second;
            
            // push a version in case the transaction fails
            ledger.pushVersion ();
            
            // TODO: if there's an error, delete the transaction and stop processing
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
string TransactionQueue::getTransactionNote ( string accountName, u64 nonce ) const {

    map < string, MakerQueue >::const_iterator makerIt = this->mDatabase.find ( accountName );
    if ( makerIt != this->mDatabase.cend ()) {
        const MakerQueue& queue = makerIt->second;
        map < u64, shared_ptr < Transaction >>::const_iterator transactionIt = queue.find ( nonce );
        if ( transactionIt != queue.cend ()) {
            return transactionIt->second->getNote ();
        }
    }
    return "";
}

//----------------------------------------------------------------//
void TransactionQueue::pruneTransactions ( const Chain& chain ) {

    // TODO: fix this brute force
    map < string, MakerQueue >::iterator makerItCursor = this->mDatabase.begin ();
    while ( makerItCursor != this->mDatabase.end ()) {
        map < string, MakerQueue >::iterator makerIt = makerItCursor++;
    
        shared_ptr < Account > account = chain.getAccount ( makerIt->first );
        if ( account ) {
        
            u64 nonce = account->getNonce ();
            MakerQueue& queue = makerIt->second;
        
            map < u64, shared_ptr < Transaction >>::iterator queueItCursor = queue.begin ();
            while ( queueItCursor != queue.end ()) {
            
                map < u64, shared_ptr < Transaction >>::iterator queueIt = queueItCursor++;
            
                if ( queueIt->first < nonce ) {
                    queue.erase ( queueIt );
                }
            }
            if ( queue.size () > 0 ) continue; // skip erasing the queue
        }
        this->mDatabase.erase ( makerIt );
    }
}

//----------------------------------------------------------------//
bool TransactionQueue::pushTransaction ( shared_ptr < Transaction > transaction ) {

    const TransactionMaker* maker = transaction->getMaker ();
    if ( !maker ) return false;

    MakerQueue& queue = this->mDatabase [ maker->getAccountName ()];
    queue [ maker->getNonce ()] = transaction;
    return true;
}

//----------------------------------------------------------------//
void TransactionQueue::reset () {

    this->mDatabase.clear ();
}

//----------------------------------------------------------------//
TransactionQueue::TransactionQueue () {
}

//----------------------------------------------------------------//
TransactionQueue::~TransactionQueue () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
