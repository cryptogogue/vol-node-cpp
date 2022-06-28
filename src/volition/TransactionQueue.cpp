// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AccountODBM.h>
#include <volition/Block.h>
#include <volition/Transaction.h>
#include <volition/TransactionQueue.h>

namespace Volition {

//================================================================//
// MakerQueueInfo
//================================================================//
class MakerQueueInfo {
public:

    AccountID       mAccountIndex;
    u64             mNonce;

    //----------------------------------------------------------------//
    MakerQueueInfo () :
        mAccountIndex ( AccountID::NULL_INDEX ),
        mNonce ( 0 ) {
    }
};

//================================================================//
// TransactionQueue
//================================================================//

//----------------------------------------------------------------//
void TransactionQueue::acceptTransaction ( shared_ptr < const Transaction > transaction ) {

    LGN_LOG_SCOPE ( VOL_FILTER_TRANSACTION_QUEUE, INFO, __PRETTY_FUNCTION__ );
    
    this->mDatabase [ transaction->getMakerAccountName ()].pushTransaction ( transaction );
}

//----------------------------------------------------------------//
void TransactionQueue::fillBlock ( Ledger& chain, Block& block, Block::VerificationPolicy policy, u64 minimumGratuity ) {

    LGN_LOG_SCOPE ( VOL_FILTER_TRANSACTION_QUEUE, INFO, __PRETTY_FUNCTION__ );

    Ledger ledger = chain;

    this->pruneTransactions ( ledger );

    ledger.setRelease ( block.getRelease ());

    const u64 maxBlockWeight    = ledger.getMaxBlockWeight ();
    u64 blockHeight             = block.getHeight ();
    u64 blockWeight             = 0;
    u64 transactionIndex        = 0;

    map < string, MakerQueueInfo > infoCache;

    bool more = true;
    while ( more ) {

        more = false;

        MakerQueueIt makerQueueItCursor = this->mDatabase.begin ();
        while ( makerQueueItCursor != this->mDatabase.end ()) {
            MakerQueueIt makerQueueIt = makerQueueItCursor++;
            
            string accountName      = makerQueueIt->first;
            MakerQueue& makerQueue  = makerQueueIt->second;
            bool byPosition         = accountName == "";
            
            // skip if there's a cached error or if there aren't any transactions
            if ( makerQueue.isBlocked ()) continue;
            if ( !makerQueue.hasTransactions ()) continue;
          
            MakerQueueInfo info = infoCache [ accountName ];
          
            // make sure the account exists
            if (( info.mAccountIndex == AccountID::NULL_INDEX ) && !byPosition ) {
                info.mAccountIndex = ledger.getAccountID ( accountName );
                if ( info.mAccountIndex == AccountID::NULL_INDEX ) {
                    this->mDatabase.erase ( makerQueueIt );
                    continue;
                }
                info.mNonce = AccountODBM ( ledger, info.mAccountIndex ).mTransactionNonce.get ();
                infoCache [ accountName ] = info;
            }
            
            // make sure transactions are ordered correctly
            if ( !byPosition ) {
                TransactionResult result = makerQueue.checkTransactionOrder ( info.mNonce );
                if ( !result ) {
                    makerQueue.setTransactionResult ( result );
                    continue;
                }
            }
            
            // get the next transaction
            shared_ptr < const Transaction > transaction = makerQueue.getTransaction ( info.mNonce );
            if ( !transaction ) {
                continue;
            }
            
            // check the gratuity
            u64 gratuity = transaction->getGratuity ();
            u64 expectedGratuity = transaction->getWeight () * minimumGratuity;
            if ( gratuity < expectedGratuity ) {
                makerQueue.ignoreTransaction ( Format::write ( "Transaction gratuity of %d less than minimum gratuity of %d.", gratuity, expectedGratuity ), transaction->getUUID ());
                continue;
            }
            
            // check the weight
            u64 transactionWeight = transaction->getWeight ();
            if ( maxBlockWeight < transactionWeight ) {
                TransactionResult result ( Format::write ( "Transaction weight of %d exceeds maximum block size of %d.", transactionWeight, maxBlockWeight ));
                result.setTransactionDetails ( *transaction );
                makerQueue.setTransactionResult ( result );
                continue;
            }
            
            // skip if block weight exceeded
            if (( blockWeight + transactionWeight ) > maxBlockWeight ) continue;
            
            // push a version in case the transaction fails
            ledger.pushVersion ();
            
            TransactionResult result = transaction->apply ( ledger, blockHeight, transactionIndex, block.getTime (), policy );
            
            if ( result ) {
                
                // transaction succeeded!
                block.pushTransaction ( transaction );
                
                info.mNonce++;
                infoCache [ accountName ] = info;
                blockWeight += transactionWeight;
                transactionIndex++;
                
                more = ( more || ( bool )makerQueue.getTransaction ( info.mNonce ));
            }
            else {
                makerQueue.setTransactionResult ( result );
                ledger.popVersion ();
            }
        }
    }
}

//----------------------------------------------------------------//
TransactionStatus TransactionQueue::getLastStatus ( string accountName ) const {

    const MakerQueue* makerQueue = this->getMakerQueueOrNull ( accountName );
    if ( makerQueue ) {
        return makerQueue->mTransactionStatus;
    }
    return TransactionStatus ();
}

//----------------------------------------------------------------//
const TransactionMakerQueue* TransactionQueue::getMakerQueueOrNull ( string accountName ) const {

    MakerQueueConstIt makerQueueIt = this->mDatabase.find ( accountName );
    if ( makerQueueIt != this->mDatabase.end ()) {
        return &makerQueueIt->second;
    }
    return NULL;
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
bool TransactionQueue::isBlocked ( string accountName ) const {

    const MakerQueue* makerQueue = this->getMakerQueueOrNull ( accountName );
    if ( makerQueue ) {
        return makerQueue->isBlocked ();
    }
    return false;
}

//----------------------------------------------------------------//
void TransactionQueue::pruneTransactions ( const AbstractLedger& chain ) {

    LGN_LOG_SCOPE ( VOL_FILTER_TRANSACTION_QUEUE, INFO, __PRETTY_FUNCTION__ );

    const AbstractLedger& ledger = chain;

    // TODO: fix this brute force
    MakerQueueIt makerQueueItCursor = this->mDatabase.begin ();
    while ( makerQueueItCursor != this->mDatabase.end ()) {
        MakerQueueIt makerQueueIt = makerQueueItCursor++;
        
        string accountName = makerQueueIt->first;
        MakerQueue& makerQueue = makerQueueIt->second;
    
        AccountID accountID = ledger.getAccountID ( accountName );
        if ( accountID == AccountID::NULL_INDEX ) {
            LGN_LOG ( VOL_FILTER_TRANSACTION_QUEUE, INFO, "pruning makerless queue" );
            makerQueue.prune ( ledger );
        }
        else {
            LGN_LOG ( VOL_FILTER_TRANSACTION_QUEUE, INFO, "pruning account queue: %s", accountName.c_str ());
            makerQueue.prune ( AccountODBM ( ledger, accountID ).mTransactionNonce.get ());
        }
        
        if ( makerQueue.isBlocked ()) continue;
        if ( makerQueue.hasTransactions ()) continue;
        
        LGN_LOG ( VOL_FILTER_TRANSACTION_QUEUE, INFO, "erasing empty account queue: %s", accountName.c_str ());
        this->mDatabase.erase ( makerQueueIt );
    }
}

//----------------------------------------------------------------//
void TransactionQueue::pushTransaction ( shared_ptr < const Transaction > transaction ) {

    LGN_LOG_SCOPE ( VOL_FILTER_TRANSACTION_QUEUE, INFO, __PRETTY_FUNCTION__ );

    if ( transaction ) {
        this->acceptTransaction ( transaction );
    }
}

//----------------------------------------------------------------//
void TransactionQueue::reset () {

    this->mDatabase.clear ();
}

//----------------------------------------------------------------//
TransactionStatus TransactionQueue::getTransactionStatus ( const AbstractLedger& ledger, string accountName, string uuid ) const {

    LGN_LOG_SCOPE ( VOL_FILTER_TRANSACTION_QUEUE, INFO, __PRETTY_FUNCTION__ );

    LGN_LOG ( VOL_FILTER_TRANSACTION_QUEUE, INFO, "account: %s uuid: %s", accountName.c_str (), uuid.c_str ());

    if ( this->isBlocked ( accountName )) {
        LGN_LOG ( VOL_FILTER_TRANSACTION_QUEUE, INFO, "BLOCKED" );
        return this->getLastStatus ( accountName );
    }

    if ( ledger.hasTransaction ( accountName, uuid )) {
        LGN_LOG ( VOL_FILTER_TRANSACTION_QUEUE, INFO, "APPLIED" );
        return TransactionStatus ( TransactionStatus::ACCEPTED, "Transaction was accepted and applied.", uuid );
    }

    if ( this->hasTransaction ( accountName, uuid )) {
        LGN_LOG ( VOL_FILTER_TRANSACTION_QUEUE, INFO, "PENDING" );
        return TransactionStatus ( TransactionStatus::PENDING, "Transaction is pending in queue.", uuid );
    }
    
    LGN_LOG ( VOL_FILTER_TRANSACTION_QUEUE, INFO, "UNKNOWN" );
    return TransactionStatus ( TransactionStatus::UNKNOWN, "Transaction is unknown.", uuid );
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
