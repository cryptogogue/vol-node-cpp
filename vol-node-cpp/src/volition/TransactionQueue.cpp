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

//----------------------------------------------------------------//
shared_ptr < const Transaction > MakerQueue::getTransaction ( u64 nonce ) const {

    TransactionConstIt transactionIt = this->mQueue.find ( nonce );
    return transactionIt != this->mQueue.cend () ? transactionIt->second : NULL;
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

    return ( this->mQueue.size () > 0 );
}

//----------------------------------------------------------------//
MakerQueue::MakerQueue () :
    mLastResult ( true ) {
}

//----------------------------------------------------------------//
bool MakerQueue::pushTransaction ( shared_ptr < const Transaction > transaction ) {

    const TransactionMaker* maker = transaction->getMaker ();
    if ( !maker ) return false;

    this->mQueue [ maker->getNonce ()] = transaction;
    this->mLastResult = TransactionResult ( true );
    return true;
}

//----------------------------------------------------------------//
void MakerQueue::prune ( u64 nonce ) {

    TransactionIt transactionItCursor = this->mQueue.begin ();
    while ( transactionItCursor != this->mQueue.end ()) {

        TransactionIt transactionIt = transactionItCursor++;

        if ( transactionIt->first < nonce ) {
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
                // get the nonce
                info.mNonce = ledger.getAccountNonce ( info.mAccountIndex );
            }
            
            // get the next transaction
            shared_ptr < const Transaction > transaction = makerQueue.getTransaction ( info.mNonce );
            if ( !transaction ) continue; // skip if no transaction
            
            // push a version in case the transaction fails
            ledger.pushVersion ();
            
            TransactionResult result = transaction->apply ( ledger, schemaHandle );
            if ( result ) {
                // transaction succeeded!
                block.pushTransaction ( transaction );
                info.mNonce++;
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
string TransactionQueue::getTransactionNote ( string accountName, u64 nonce ) const {

    const MakerQueue* makerQueue = this->getMakerQueueOrNull ( accountName );
    if ( makerQueue ) {
        shared_ptr < const Transaction > transaction = makerQueue->getTransaction ( nonce );
        if ( transaction ) {
            return transaction->getNote ();
        }
    }
    return "";
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
void TransactionQueue::pruneTransactions ( const Chain& chain ) {

    const Ledger& ledger = chain;

    // TODO: fix this brute force
    MakerQueueIt makerQueueItCursor = this->mDatabase.begin ();
    while ( makerQueueItCursor != this->mDatabase.end ()) {
        MakerQueueIt makerQueueIt = makerQueueItCursor++;
    
        string accountName = makerQueueIt->first;
        MakerQueue& makerQueue = makerQueueIt->second;
    
        Account::Index accountIndex = ledger.getAccountIndex ( accountName );
        if ( accountIndex == Account::NULL_INDEX ) {
    
            u64 nonce = ledger.getAccountNonce ( accountIndex );
            makerQueue.prune ( nonce );
        
            if ( makerQueue.hasError ()) continue;
            if ( makerQueue.hasTransactions ()) continue;
        }
        this->mDatabase.erase ( makerQueueIt );
    }
}

//----------------------------------------------------------------//
bool TransactionQueue::pushTransaction ( shared_ptr < const Transaction > transaction ) {

    const TransactionMaker* maker = transaction->getMaker ();
    if ( !maker ) return false;
    
    return this->mDatabase [ maker->getAccountName ()].pushTransaction ( transaction );
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
