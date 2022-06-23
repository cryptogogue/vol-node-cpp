// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AccountODBM.h>
#include <volition/Block.h>
#include <volition/Transaction.h>
#include <volition/TransactionMakerQueue.h>

namespace Volition {

//================================================================//
// TransactionMakerQueue
//================================================================//

//----------------------------------------------------------------//
TransactionResult TransactionMakerQueue::checkTransactionOrder ( u64 nonce ) const {

    TransactionQueueConstIt transactionIt = this->mQueue.cbegin ();
    for ( ; transactionIt != this->mQueue.cend (); ++transactionIt ) {
    
        shared_ptr < const Transaction > transaction = transactionIt->second;
        u64 transactionNonce = transaction->getNonce ();
    
        if ( transactionNonce > nonce ) {
            TransactionResult result = Format::write ( "Transactions submitted out of order. Nonce is %d, but next transaction in queue is %d.", ( int )nonce, ( int )transactionNonce );
            result.setTransactionDetails ( *transaction );
            return result;
        }
    }
    return true;
}

//----------------------------------------------------------------//
shared_ptr < const Transaction > TransactionMakerQueue::getTransaction ( u64 positionOrNonce ) const {

    TransactionQueueConstIt transactionIt = this->isMakerless () ? std::next ( this->mQueue.begin (), ( int )positionOrNonce ) : this->mQueue.find ( positionOrNonce );
    return transactionIt != this->mQueue.cend () ? transactionIt->second : NULL;
}

//----------------------------------------------------------------//
shared_ptr < const Transaction > TransactionMakerQueue::getTransaction ( string uuid ) const {

    TransactionLookupConstIt transactionIt = this->mLookup.find ( uuid );
    return transactionIt != this->mLookup.cend () ? transactionIt->second : NULL;
}

////----------------------------------------------------------------//
//bool TransactionMakerQueue::hasTransaction ( u64 positionOrNonce ) const {
//
//    return this->isMakerless () ? ( positionOrNonce < this->mQueue.size ()) : ( this->mQueue.find ( positionOrNonce ) != this->mQueue.end ());
//}

//----------------------------------------------------------------//
bool TransactionMakerQueue::hasTransactions () const {

    return ( this->mQueue.size () > 0 );
}

//----------------------------------------------------------------//
void TransactionMakerQueue::ignoreTransaction ( string message, string uuid ) {

    this->mQueueStatus = BLOCKED_ON_IGNORE;
    this->mTransactionStatus = TransactionStatus ( TransactionStatus::IGNORED, message, uuid );
}

//----------------------------------------------------------------//
bool TransactionMakerQueue::isBlocked () const {

    return ( this->mQueueStatus != STATUS_OK );
}

//----------------------------------------------------------------//
TransactionMakerQueue::TransactionMakerQueue () :
    mCount ( 0 ) {
    
    this->setTransactionResult ( true );
}

//----------------------------------------------------------------//
void TransactionMakerQueue::pushTransaction ( shared_ptr < const Transaction > transaction ) {

    LGN_LOG_SCOPE ( VOL_FILTER_TRANSACTION_QUEUE, INFO, __PRETTY_FUNCTION__ );

    if ( this->mCount == 0 ) {
        this->mAccountName = transaction->getMakerAccountName ();
    }
    else {
        assert ( this->mAccountName == transaction->getMakerAccountName ());
    }

    u64 nonce = this->isMakerless () ? this->mCount : transaction->getNonce ();

    this->mQueue [ nonce ] = transaction;
    this->mLookup [ transaction->getUUID ()] = transaction;

    if ( !this->isMakerless ()) {
    
        // transactions *must* be submitted in order; erase any later transactions submitted before current nonce
        TransactionQueueIt transactionItCursor = this->mQueue.find ( nonce )++;
        while ( transactionItCursor != this->mQueue.end ()) {
            
            TransactionQueueIt transactionIt = transactionItCursor++;
            
            if ( transactionIt->first > nonce ) {
                this->mLookup.erase ( transactionIt->second->getUUID ());
                this->mQueue.erase ( transactionIt );
            }
        }
    }

    this->setTransactionResult ( true );
    this->mCount++;
}

//----------------------------------------------------------------//
void TransactionMakerQueue::prune ( u64 nonce ) {

    LGN_LOG_SCOPE ( VOL_FILTER_TRANSACTION_QUEUE, INFO, __PRETTY_FUNCTION__ );

    TransactionQueueIt transactionItCursor = this->mQueue.begin ();
    while ( transactionItCursor != this->mQueue.end ()) {

        TransactionQueueIt transactionIt = transactionItCursor++;

        LGN_LOG ( VOL_FILTER_TRANSACTION_QUEUE, INFO, "nonce (account): %d nonce (transaction): %d", ( int )nonce, ( int )transactionIt->first );

        if ( transactionIt->first < nonce ) {
            this->mLookup.erase ( transactionIt->second->getUUID ());
            this->mQueue.erase ( transactionIt );
        }
    }
}

//----------------------------------------------------------------//
void TransactionMakerQueue::prune ( const AbstractLedger& ledger ) {
    
    TransactionQueueIt transactionItCursor = this->mQueue.begin ();
    while ( transactionItCursor != this->mQueue.end ()) {

        TransactionQueueIt transactionIt = transactionItCursor++;
        shared_ptr < const Transaction > transaction = transactionIt->second;

        if ( transaction->wasApplied ( ledger )) {
            this->mLookup.erase ( transactionIt->second->getUUID ());
            this->mQueue.erase ( transactionIt );
        }
    }
}

//----------------------------------------------------------------//
void TransactionMakerQueue::setTransactionResult ( TransactionResult result ) {

    if ( result ) {
        this->mQueueStatus = STATUS_OK;
        this->mTransactionStatus = TransactionStatus ();
    }
    else {
        this->mQueueStatus = BLOCKED_ON_ERROR;
        this->mTransactionStatus = TransactionStatus (
            TransactionStatus::REJECTED,
            result.getMessage (),
            result.getUUID ()
        );
        this->mQueue.clear ();
    }
}

} // namespace Volition
