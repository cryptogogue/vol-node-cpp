// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractChainRecorder.h>
#include <volition/Block.h>
#include <volition/TransactionQueue.h>
#include <volition/TheContext.h>
#include <volition/SyncChainTask.h>

namespace Volition {

//================================================================//
// MakerQueueInfo
//================================================================//
class MakerQueueInfo {
public:

    Account::Index  mAccountIndex;
    u64             mNonce;

    //----------------------------------------------------------------//
    MakerQueueInfo () :
        mAccountIndex ( Account::NULL_INDEX ),
        mNonce ( 0 ) {
    }
};

//================================================================//
// MakerQueue
//================================================================//

////----------------------------------------------------------------//
//shared_ptr < const Transaction > MakerQueue::getTransaction ( u64 nonce ) const {
//
//    TransactionQueueConstIt transactionIt = this->mQueue.find ( nonce );
//    return transactionIt != this->mQueue.cend () ? transactionIt->second : NULL;
//}

//----------------------------------------------------------------//
shared_ptr < const Transaction > MakerQueue::getTransaction ( string uuid ) const {

    if ( this->mControl && ( this->mControl->getUUID () == uuid )) {
        return this->mControl;
    }
    TransactionLookupConstIt transactionIt = this->mLookup.find ( uuid );
    return transactionIt != this->mLookup.cend () ? transactionIt->second : NULL;
}

//----------------------------------------------------------------//
bool MakerQueue::hasError () const {

    return ( !this->mLastResult );
}

//----------------------------------------------------------------//
bool MakerQueue::hasTransaction ( u64 nonce ) const {

    return ( this->mQueue.find ( nonce ) != this->mQueue.end ());
}


//----------------------------------------------------------------//
bool MakerQueue::hasTransactions () const {

    return ( this->mControl || ( this->mQueue.size () > 0 ));
}

//----------------------------------------------------------------//
MakerQueue::MakerQueue () :
    mLastResult ( true ) {
}

//----------------------------------------------------------------//
shared_ptr < const Transaction > MakerQueue::nextTransaction ( u64 nonce ) const {

    if ( this->mControl && ( nonce <= this->mControl->getNonce ())) {
        return this->mControl;
    }
    TransactionQueueConstIt transactionIt = this->mQueue.find ( nonce );
    return transactionIt != this->mQueue.cend () ? transactionIt->second : NULL;
}

//----------------------------------------------------------------//
void MakerQueue::pushTransaction ( shared_ptr < const Transaction > transaction ) {

    if ( this->mControl ) return;

    if ( transaction->needsControl ()) {
    
        this->mQueue.clear ();
        this->mLookup.clear ();
    
        this->mControl = transaction;
    }
    else {
        this->mQueue [ transaction->getNonce ()] = transaction;
        this->mLookup [ transaction->getUUID ()] = transaction;
    }
}

//----------------------------------------------------------------//
void MakerQueue::prune ( u64 nonce ) {
    
    if ( this->mControl && ( this->mControl->getNonce () < nonce )) {
        this->mControl = NULL;
    }

    TransactionQueueIt transactionItCursor = this->mQueue.begin ();
    while ( transactionItCursor != this->mQueue.end ()) {

        TransactionQueueIt transactionIt = transactionItCursor++;

        if ( transactionIt->first < nonce ) {
            this->mLookup.erase ( transactionIt->second->getUUID ());
            this->mQueue.erase ( transactionIt );
        }
    }
}

//----------------------------------------------------------------//
void MakerQueue::setError ( TransactionResult error ) {

    this->mLastResult = error;
    if ( !error ) {
        this->mQueue.clear ();
    }
}

//================================================================//
// TransactionQueue
//================================================================//

//----------------------------------------------------------------//
void TransactionQueue::fillBlock ( Chain& chain, Block& block ) {

    Ledger ledger;
    ledger.takeSnapshot ( chain );
    SchemaHandle schemaHandle ( ledger );

    const u64 maxBlockSize = ledger.getBlockSize ();
    u64 blockSize = 0;

    map < string, MakerQueueInfo > infoCache;

    bool more = true;
    while ( more ) {

        more = false;

        MakerQueueIt makerQueueItCursor = this->mDatabase.begin ();
        while ( makerQueueItCursor != this->mDatabase.end ()) {
            MakerQueueIt makerQueueIt = makerQueueItCursor++;
            
            string accountName = makerQueueIt->first;
            MakerQueue& makerQueue = makerQueueIt->second;
          
            // skip if there's a cached error or if there aren't any transactions
            if ( makerQueue.hasError ()) continue;
            if ( !makerQueue.hasTransactions ()) continue;
          
            MakerQueueInfo info = infoCache [ accountName ];
          
            // make sure the account exists
            if ( info.mAccountIndex == Account::NULL_INDEX ) {
                info.mAccountIndex = ledger.getAccountIndex ( accountName );
                if ( info.mAccountIndex == Account::NULL_INDEX ) {
                    this->mDatabase.erase ( makerQueueIt );
                    continue;
                }
                info.mNonce = ledger.getAccountTransactionNonce ( info.mAccountIndex );
                infoCache [ accountName ] = info;
            }
            
            // get the next transaction
            shared_ptr < const Transaction > transaction = makerQueue.nextTransaction ( info.mNonce );
            if ( !transaction ) continue; // skip if no transaction
            
            u64 transactionSize = transaction->weight ();
            if (( blockSize + transactionSize ) > maxBlockSize ) continue;
            
            // push a version in case the transaction fails
            ledger.pushVersion ();
            
            TransactionResult result = transaction->apply ( ledger, block.getTime (), schemaHandle );
            if ( result ) {
                // transaction succeeded!
                block.pushTransaction ( transaction );
                
                if ( !transaction->needsControl ()) {
                    blockSize += transactionSize;
                }
                info.mNonce = transaction->getNonce () + 1;
                infoCache [ accountName ] = info;
                
                more = ( more || makerQueue.hasTransaction ( info.mNonce ));
            }
            else {
                makerQueue.setError ( result );
                ledger.popVersion ();
            }
        }
    }
}

//----------------------------------------------------------------//
const MakerQueue* TransactionQueue::getMakerQueueOrNull ( string accountName ) const {

    MakerQueueConstIt makerQueueIt = this->mDatabase.find ( accountName );
    if ( makerQueueIt != this->mDatabase.end ()) {
        return &makerQueueIt->second;
    }
    return NULL;
}


//----------------------------------------------------------------//
TransactionResult TransactionQueue::getLastResult ( string accountName ) const {

    const MakerQueue* makerQueue = this->getMakerQueueOrNull ( accountName );
    if ( makerQueue ) {
        return makerQueue->mLastResult;
    }
    return true;
}

//----------------------------------------------------------------//
bool TransactionQueue::hasError ( string accountName ) {

    const MakerQueue* makerQueue = this->getMakerQueueOrNull ( accountName );
    if ( makerQueue ) {
        return makerQueue->hasError ();
    }
    return false;
}

//----------------------------------------------------------------//
bool TransactionQueue::hasTransaction ( string accountName, string uuid ) const {

    const MakerQueue* makerQueue = this->getMakerQueueOrNull ( accountName );
    if ( makerQueue ) {
        shared_ptr < const Transaction > transaction = makerQueue->getTransaction ( uuid );
        if ( transaction ) return true;
    }
    return false;
}

//----------------------------------------------------------------//
void TransactionQueue::pruneTransactions ( const Chain& chain ) {

    const Ledger& ledger = chain;

    // TODO: fix this brute force
    MakerQueueIt makerQueueItCursor = this->mDatabase.begin ();
    while ( makerQueueItCursor != this->mDatabase.end ()) {
        MakerQueueIt makerQueueIt = makerQueueItCursor++;
    
        string accountName = makerQueueIt->first;
        MakerQueue& makerQueue = makerQueueIt->second;
    
        Account::Index accountIndex = ledger.getAccountIndex ( accountName );
        if ( accountIndex != Account::NULL_INDEX ) {
            
            u64 nonce = ledger.getAccountTransactionNonce ( accountIndex );
            makerQueue.prune ( nonce );
        
            if ( makerQueue.hasError ()) continue;
            if ( makerQueue.hasTransactions ()) continue;
        }
        this->mDatabase.erase ( makerQueueIt );
    }
}

//----------------------------------------------------------------//
void TransactionQueue::pushTransaction ( shared_ptr < const Transaction > transaction ) {

    const TransactionMaker* maker = transaction->getMaker ();
    assert ( maker );
    
    this->mDatabase [ maker->getAccountName ()].pushTransaction ( transaction );
}

//----------------------------------------------------------------//
void TransactionQueue::reset () {

    this->mDatabase.clear ();
}

//----------------------------------------------------------------//
void TransactionQueue::setError ( shared_ptr < const Transaction > transaction, TransactionResult error ) {

    const TransactionMaker* maker = transaction->getMaker ();
    if ( !maker ) return;

    return this->mDatabase [ maker->getAccountName ()].setError ( error );
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
