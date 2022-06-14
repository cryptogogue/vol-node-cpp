// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_GENESIS_H
#define VOLITION_TRANSACTIONS_GENESIS_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Accessors.h>
#include <volition/MinerInfo.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// ConsensusSettings
//================================================================//
class ConsensusSettings :
    public virtual AbstractSerializable {
public:

    string          mIdentity;
    u64             mMaxBlockWeight;
    time_t          mBlockDelayInSeconds;
    time_t          mRewriteWindowInSeconds;

    GET_SET ( string,           Identity,                       mIdentity )
    GET_SET ( u64,              MaxBlockWeight,                 mMaxBlockWeight )
    GET_SET ( time_t,           BlockDelayInSeconds,            mBlockDelayInSeconds )
    GET_SET ( time_t,           RewriteWindowInSeconds,         mRewriteWindowInSeconds )

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        u64 blockDelay      = ( u64 )this->mBlockDelayInSeconds;
        u64 rewriteWindow   = ( u64 )this->mRewriteWindowInSeconds;
        
        serializer.serialize ( "identity",                  this->mIdentity );
        serializer.serialize ( "maxBlockWeight",            this->mMaxBlockWeight );
        serializer.serialize ( "blockDelayInSeconds",       blockDelay );
        serializer.serialize ( "rewriteWindowInSeconds",    rewriteWindow );
        
        this->mBlockDelayInSeconds      = ( time_t )blockDelay;
        this->mRewriteWindowInSeconds   = ( time_t )rewriteWindow;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        u64 blockDelay      = ( u64 )this->mBlockDelayInSeconds;
        u64 rewriteWindow   = ( u64 )this->mRewriteWindowInSeconds;
        
        serializer.serialize ( "identity",                  this->mIdentity );
        serializer.serialize ( "maxBlockWeight",            this->mMaxBlockWeight );
        serializer.serialize ( "blockDelayInSeconds",       blockDelay );
        serializer.serialize ( "rewriteWindowInSeconds",    rewriteWindow );
    }
    
    //----------------------------------------------------------------//
    TransactionResult apply ( AbstractLedger& ledger ) const {
    
        if ( this->mIdentity.size () == 0 )         return "Missing network identity string.";
        if ( this->mMaxBlockWeight == 0 )           return "Max block weight must be non-zero.";
        if ( this->mBlockDelayInSeconds == 0 )      return "Block delay must be non-zero.";
        if ( this->mRewriteWindowInSeconds == 0 )   return "Rewrite window must be non-zero.";
    
        ledger.setIdentity ( this->mIdentity );
    
        ledger.setValue < u64 >( Ledger::keyFor_maxBlockWeight (),  this->mMaxBlockWeight );
        ledger.setValue < u64 >( Ledger::keyFor_blockDelay (),      ( u64 )this->mBlockDelayInSeconds );
        ledger.setValue < u64 >( Ledger::keyFor_rewriteWindow (),   ( u64 )this->mRewriteWindowInSeconds );
        
        return true;
    }
    
    //----------------------------------------------------------------//
    ConsensusSettings () :
        mMaxBlockWeight ( 0 ),
        mBlockDelayInSeconds ( 0 ),
        mRewriteWindowInSeconds ( 0 ) {
    }
};

//================================================================//
// GenesisAccount
//================================================================//
class GenesisAccount :
    public AbstractSerializable {
public:

    string              mName;
    CryptoPublicKey     mKey;
    u64                 mGrant;
    SerializableSharedConstPtr < MinerInfo > mMinerInfo;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "name",          this->mName );
        serializer.serialize ( "grant",         this->mGrant  );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "minerInfo",     this->mMinerInfo );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "name",          this->mName );
        serializer.serialize ( "grant",         this->mGrant  );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "minerInfo",     this->mMinerInfo );
    }
};

//================================================================//
// Genesis
//================================================================//
class Genesis :
    public ConsensusSettings,
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "GENESIS" )
    TRANSACTION_WEIGHT ( 0 )
    TRANSACTION_MATURITY ( 0 )

    u64                                     mTotalVOL;
    u64                                     mPrizePool;
    
    SerializableVector < GenesisAccount >   mAccounts;

    //----------------------------------------------------------------//
    void pushAccount ( const GenesisAccount& genesisAccount ) {
    
        this->mAccounts.push_back ( genesisAccount );
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        ConsensusSettings::AbstractSerializable_serializeFrom ( serializer );
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        this->mTotalVOL     = 100000000000;
        this->mPrizePool    = 0;
        
        serializer.serialize ( "totalVOL",                  this->mTotalVOL );
        serializer.serialize ( "prizePool",                 this->mPrizePool );
        serializer.serialize ( "accounts",                  this->mAccounts );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        ConsensusSettings::AbstractSerializable_serializeTo ( serializer );
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "totalVOL",                  this->mTotalVOL );
        serializer.serialize ( "prizePool",                 this->mPrizePool );
        serializer.serialize ( "accounts",                  this->mAccounts );
    }
    
    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        UNUSED ( context );
        return false;
    }
    
    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_genesis ( AbstractLedger& ledger ) const override {
        
        u64 accountVOL = 0;
        
        for ( size_t i = 0; i < this->mAccounts.size (); ++i ) {
            const GenesisAccount& account = this->mAccounts [ i ];
                        
            if ( !ledger.newAccount ( account.mName, account.mGrant, Ledger::MASTER_KEY_NAME, account.mKey, Policy (), Policy ())) return false;
            if ( account.mMinerInfo ) {
                if ( !ledger.registerMiner (
                    ledger.getAccountID ( account.mName ),
                    *account.mMinerInfo,
                    false
                )) return false;
            }
            accountVOL += account.mGrant;
        }
        
        u64 premined = accountVOL + this->mPrizePool;
        
        if ( this->mTotalVOL < premined ) return false;
        
        // miner rewards
        LedgerFieldODBM < u64 > rewardPoolField ( ledger, Ledger::keyFor_rewardPool ());
        rewardPoolField.set ( this->mTotalVOL - premined );
        
        // prizes (redeemables)
        LedgerFieldODBM < u64 > prizePoolField ( ledger, Ledger::keyFor_prizePool ());
        prizePoolField.set ( this->mPrizePool );
        
        // total vol in ecosystem
        LedgerFieldODBM < u64 > totalVOLField ( ledger, Ledger::keyFor_totalVOL ());
        totalVOLField.set ( this->mTotalVOL );
        
        return this->ConsensusSettings::apply ( ledger );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
