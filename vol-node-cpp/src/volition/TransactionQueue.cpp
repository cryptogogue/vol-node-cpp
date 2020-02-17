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

    // visit each account
    map < string, MakerQueue >::iterator makerQueueIt = this->mDatabase.begin ();
    for ( ; makerQueueIt != this->mDatabase.end (); ++makerQueueIt ) {
        MakerQueue& makerQueue = makerQueueIt->second;
    
        // TODO: get the account nonce and skip ahead
    
        // visit each transaction (ordered by nonce)
        map < u64, shared_ptr < Transaction >>::iterator queueIt = makerQueue.begin ();
        for ( ; queueIt != makerQueue.end (); ++queueIt ) {
            
            shared_ptr < Transaction > transaction = queueIt->second;
            
            // push a version in case the transaction fails
            ledger.pushVersion ();
            
            if ( transaction->apply ( ledger, schemaHandle )) {
                block.pushTransaction ( transaction );
            }
            else {
                // didn't like that transaction; empty the queue, revert and continue
                this->mRejected.insert ( makerQueueIt->first );
                makerQueue.clear (); // TODO: record the error
                ledger.popVersion ();
                break;
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
bool TransactionQueue::isRejected ( string accountName ) const {

    return ( this->mRejected.find ( accountName ) != this->mRejected.cend ());
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

    string accountName = maker->getAccountName ();
    this->mRejected.erase ( accountName );

    MakerQueue& queue = this->mDatabase [ accountName ];
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
