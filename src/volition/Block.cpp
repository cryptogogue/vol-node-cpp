// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/BlockODBM.h>
#include <volition/CryptoKey.h>
#include <volition/Format.h>
#include <volition/Ledger.h>
#include <volition/MonetaryPolicy.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/Transaction.h>

namespace Volition {

//================================================================//
// BlockBody
//================================================================//

//----------------------------------------------------------------//
BlockBody::BlockBody () {
}

//----------------------------------------------------------------//
BlockBody::~BlockBody () {
}

//================================================================//
// override
//================================================================//

//----------------------------------------------------------------//
void BlockBody::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    serializer.serialize ( "reward",        this->mReward );
    serializer.serialize ( "transactions",  this->mTransactions );
}

//----------------------------------------------------------------//
void BlockBody::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "reward",        this->mReward );
    serializer.serialize ( "transactions",  this->mTransactions );
}

//================================================================//
// Block
//================================================================//

//----------------------------------------------------------------//
void Block::affirmHash () {

    if ( !this->mDigest ) {
        this->mDigest = Digest ( *this );
    }
}

//----------------------------------------------------------------//
LedgerResult Block::apply ( AbstractLedger& ledger, VerificationPolicy policy ) const {
    
    LGN_LOG_SCOPE ( VOL_FILTER_BLOCK, INFO, __PRETTY_FUNCTION__ );

    if ( ledger.getVersion () != this->mHeight ) return "Apply block: height/version mismatch.";
    
    if ( this->mHeight == 0 ) {
        ledger.init ();
    }
    
    LedgerResult verifyResult = this->verify ( ledger, policy );
    if ( !verifyResult ) return verifyResult;

    ledger.expireOffers ( this->mTime );

    // some transactions need to be applied later.
    // we need to evaluate if they are legal now.
    // then process them once we have the entropy.
    
    // before applying the block, we need to apply the entropy.
    // then, get the list of blocks with transactions due on the current version.
    // apply those transactions and remove them from the pending list.
    // then, as we push the block, if it has pending transactions, add them.
    // if a block is removed or added from the list, flag it.
    // if it's been flagged, record it in the ledger at the end.

    // apply the entropy up front.
    this->applyEntropy ( ledger );

    // process unfinished blocks.
    UnfinishedBlockList unfinished = ledger.getUnfinished ();
    bool unfinishedChanged = false;
    
    UnfinishedBlockList nextUnfinished;
    UnfinishedBlockList::Iterator unfinishedBlockIt = unfinished.mBlocks.cbegin ();
    for ( ; unfinishedBlockIt != unfinished.mBlocks.end (); ++unfinishedBlockIt ) {
        UnfinishedBlock unfinishedBlock = *unfinishedBlockIt;
        
        if ( unfinishedBlock.mMaturity == this->mHeight ) {
            
            shared_ptr < const Block > block = ledger.getBlock ( unfinishedBlock.mBlockID );
            assert ( block );
            
            size_t nextMaturity;
            LedgerResult transactionsResult = block->applyTransactions ( ledger, policy, nextMaturity );
            if ( !transactionsResult ) return transactionsResult;
            
            if ( nextMaturity > this->mHeight ) {
            
                unfinishedBlock.mMaturity = nextMaturity;
                nextUnfinished.mBlocks.push_back ( unfinishedBlock );
            }
            
            unfinishedChanged = true;
        }
    }

    // apply transactions
    size_t nextMaturity;
    LedgerResult transactionsResult = this->applyTransactions ( ledger, policy, nextMaturity );
    if ( !transactionsResult ) return transactionsResult;
    
    if ( nextMaturity > this->mHeight ) {
    
        UnfinishedBlock unfinishedBlock;
        unfinishedBlock.mBlockID = this->mHeight;
        unfinishedBlock.mMaturity = nextMaturity;
        nextUnfinished.mBlocks.push_back ( unfinishedBlock );
        
        unfinishedChanged = true;
    }
    
    // check pending block list, and apply if changed.
    if ( unfinishedChanged ) {
        ledger.setUnfinished ( nextUnfinished );
    }
    
    BlockODBM blockODBM ( ledger, this->mHeight );
    
    string hash = this->mDigest.toHex ();
    
    blockODBM.mHash.set ( this->mDigest.toHex ());
    blockODBM.mPose.set ( this->mPose.toHex ());
    blockODBM.mHeader.set ( *this );
    blockODBM.mBlock.set ( *this );

    return true;
}

//----------------------------------------------------------------//
LedgerResult Block::applyTransactions ( AbstractLedger& ledger, VerificationPolicy policy, size_t& nextMaturity ) const {

    LGN_LOG_SCOPE ( VOL_FILTER_BLOCK, INFO, __PRETTY_FUNCTION__ );

    if ( !this->mBody ) return false;

    nextMaturity = this->mHeight;
    size_t height = ledger.getVersion ();

    assert ( this->mHeight == height );

    AccountODBM accountODBM ( ledger, this->mMinerID );
    assert ( accountODBM || ledger.isGenesis ());

    size_t gratuity         = 0;
    size_t profitShare      = 0;
    size_t transferTax      = 0;

    if ( ledger.getVersion () >= this->mHeight ) {
        
        LGN_LOG_SCOPE ( VOL_FILTER_BLOCK, INFO, "Apply transactions" );
        
        // apply block transactions.
        for ( size_t i = 0; i < this->mBody->mTransactions.size (); ++i ) {
            const Transaction& transaction = *this->mBody->mTransactions [ i ];
            
            size_t transactionMaturity = this->mHeight + transaction.getMaturity ();
            if ( transactionMaturity == height ) {
            
                TransactionResult result = transaction.apply ( ledger, height, this->getVersion (), i, this->mTime, policy );
                if ( !result ) return Format::write ( "%s: %s", result.getUUID ().c_str (), result.getMessage ().c_str ());
                
                gratuity        += transaction.getGratuity ();
                profitShare     += transaction.getProfitShare ();
                transferTax     += transaction.getTransferTax ();
            }
            
            if ( nextMaturity < transactionMaturity ) {
                nextMaturity = transactionMaturity;
            }
        }
    }
    
    if ( accountODBM ) {
    
        LGN_LOG_SCOPE ( VOL_FILTER_BLOCK, INFO, "Apply mining rewards" );
    
        {
            LGN_LOG_SCOPE ( VOL_FILTER_BLOCK, INFO, "Invoke reward script" );
            ledger.invokeReward ( this->mMinerID, this->mBody->mReward, this->mTime );
        }
        
        {
            LGN_LOG_SCOPE ( VOL_FILTER_BLOCK, INFO, "Apply monetary policy" );
        
            MonetaryPolicy monetaryPolicy = ledger.getMonetaryPolicy ();
            
            u64 miningReward = 0;
            u64 miningTax = 0;
            
            if ( monetaryPolicy.hasMiningReward ()) {
            
                LedgerFieldODBM < u64 > rewardPoolField ( ledger, Ledger::keyFor_rewardPool ());
                u64 rewardPool = rewardPoolField.get ();
                
                if ( rewardPool ) {
                    
                    u64 grossMiningReward   = monetaryPolicy.calculateMiningReward ( rewardPool );
                    miningTax               = monetaryPolicy.calculateMiningRewardTax ( grossMiningReward );
                    
                    rewardPoolField.set ( rewardPool - grossMiningReward );
                    miningReward = grossMiningReward - miningTax;
                }
            }
            
            u64 minerProfit = miningReward + gratuity;
            if ( minerProfit > 0 ) {
                accountODBM.addFunds ( minerProfit - profitShare );
            }
            
            ledger.payout ( miningTax + transferTax + profitShare );
        }
    }

    accountODBM.mMinerBlockCount.set ( accountODBM.mMinerBlockCount.get ( 0 ) + 1 );

    return true;
}

//----------------------------------------------------------------//
Block::Block () :
    mBodyType ( 0 ) {
}

//----------------------------------------------------------------//
Block::Block ( string bodyString ) :
    mBodyType ( 0 ),
    mBodyString ( bodyString ) {
    
    this->affirmBody ();
    this->affirmHash ();
}

//----------------------------------------------------------------//
Block::~Block () {
}

//----------------------------------------------------------------//
size_t Block::countTransactions () const {

    return this->mBody ? this->mBody->mTransactions.size () : 0;
}

//----------------------------------------------------------------//
const Transaction* Block::getTransaction ( u64 index ) const {

    return ( this->mBody && ( index < this->mBody->mTransactions.size () )) ? this->mBody->mTransactions [ index ].get () : NULL;
}

//----------------------------------------------------------------//
size_t Block::getWeight () const {

    size_t weight = 0;
    if ( this->mBody ) {
        for ( size_t i = 0; i < this->mBody->mTransactions.size (); ++i ) {
            weight += this->mBody->mTransactions [ i ]->getWeight ();
        }
    }
    return weight;
}

//----------------------------------------------------------------//
void Block::pushTransaction ( shared_ptr < const Transaction > transaction ) {

    this->affirmBody ();
    this->mBody->mTransactions.push_back ( transaction );
}

//----------------------------------------------------------------//
void Block::setReward ( string reward ) {

    this->affirmBody ();
    this->mBody->mReward = reward;
}

//----------------------------------------------------------------//
const Digest& Block::sign ( const CryptoKeyPair& key, string hashAlgorithm ) {
        
    this->mDigest = Digest ( *this );
    this->mSignature = key.sign ( this->mDigest, hashAlgorithm );
    return this->mSignature.getSignature ();
}

//----------------------------------------------------------------//
LedgerResult Block::verify ( const AbstractLedger& ledger, VerificationPolicy policy ) const {

    if ( this->mHeight == 0 ) {
        BlockODBM genesisODBM ( ledger, 0 );
        if ( genesisODBM && ( genesisODBM.mHash.get () != this->mDigest.toHex ())) return "Verify block: genesis hash mismatch.";
        return true;
    }
    else {
    
        BlockODBM parentODBM ( ledger, this->mHeight - 1 );
        if ( parentODBM.mHash.get ( "" ) != this->mPrevDigest.toHex ()) return "Verify block: parent hash mismatch.";

        if ( this->mBlockDelay != ledger.getBlockDelayInSeconds ()) return "Verify block: block delay mismatch.";
        if ( this->mRewriteWindow != ledger.getRewriteWindowInSeconds ()) return "Verify block: rewrite window mismatch.";
        if ( this->getWeight () > ledger.getMaxBlockWeight ()) return "Verify block: max block weight exceeded.";
    }

    shared_ptr < const MinerInfo > minerInfo = AccountODBM ( ledger, this->mMinerID ).mMinerInfo.get ();
    if ( !minerInfo ) return "Verify block: miner info not found.";

    const CryptoPublicKey& key = minerInfo->getPublicKey ();

    if ( policy & ( VerificationPolicy::VERIFY_POSE | VerificationPolicy::VERIFY_CHARM )) {

        BlockODBM parentODBM ( ledger, this->mHeight - 1 );
        string prevPoseHex = parentODBM.mPose.get ();

        if ( policy & VerificationPolicy::VERIFY_POSE ) {
            Digest digest = this->hashPose ( prevPoseHex );
            if ( !key.verify ( this->mPose, digest )) return "Verify block: invalid POSE.";
        }

        if ( policy & VerificationPolicy::VERIFY_CHARM ) {
            
            Digest prevPose;
            prevPose.fromHex ( prevPoseHex );
            Digest charm = BlockHeader::calculateCharm ( prevPose, minerInfo->getVisage ());
            if ( this->mCharm != charm ) return "Verify block: invalid CHARM.";
        }
    }

    if ( policy & VerificationPolicy::VERIFY_BLOCK_SIG ) {
        if ( !key.verify ( this->mSignature, *this )) return "Verify block: invalid SIGNATURE.";
    }
    return true;
}

//================================================================//
// overrides
//================================================================//

 //----------------------------------------------------------------//
void Block::affirmBody () {

    if ( this->mBody ) return;

    shared_ptr < BlockBody > body = make_shared < BlockBody >();

    if ( this->mBodyString.size ()) {
        FromJSONSerializer::fromJSONString ( *body, this->mBodyString );
    }
    this->mBody = body;
}

//----------------------------------------------------------------//
void Block::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    BlockHeader::AbstractSerializable_serializeFrom ( serializer );
    
    serializer.serialize ( "bodyType",      this->mBodyType );
    serializer.serialize ( "body",          this->mBodyString );
    this->affirmBody ();
    this->affirmHash ();
}

//----------------------------------------------------------------//
void Block::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
    BlockHeader::AbstractSerializable_serializeTo ( serializer );
    
    serializer.serialize ( "bodyType",      this->mBodyType );
        
    if ( !this->mBodyString.size ()) {
        this->mBodyString = this->mBody ? ToJSONSerializer::toJSONString ( *this->mBody ) : "";
    }
    serializer.serialize ( "body",          this->mBodyString );
}

} // namespace Volition
