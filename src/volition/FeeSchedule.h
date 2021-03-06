// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_FEESCHEDULE_H
#define VOLITION_FEESCHEDULE_H

#include <volition/common.h>
#include <volition/AccountID.h>
#include <volition/KeyAndPolicy.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// FeePercent
//================================================================//
class FeePercent :
    public AbstractSerializable {
public:

    static const u64 DEFAULT_SCALE = 100;

    u64         mScale;
    u64         mPercent;               // percent of the gratuity offered to miner (in fixed point)
    
    //----------------------------------------------------------------//
    operator bool () const {
    
        return (( this->mScale > 0 ) && ( this->mPercent > 0 ));
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "scale",             this->mScale );
        serializer.serialize ( "percent",           this->mPercent );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "scale",             this->mScale );
        serializer.serialize ( "percent",           this->mPercent );
    }

    //----------------------------------------------------------------//
    bool check ( u64 amount, u64 share ) const {
        
        return ( share == this->computeAndRoundUp ( amount ));
    }
    
    //----------------------------------------------------------------//
    u64 computeAndRoundDown ( u64 amount ) const {
    
        if ( this->mScale == 0 ) return 0;
    
        u128 x = amount * this->mScale * this->mPercent;
        u128 y = this->mScale * this->mScale;
        return ( u64 )( x / y );
    }
    
    //----------------------------------------------------------------//
    u64 computeAndRoundUp ( u64 amount ) const {
        
        if ( this->mScale == 0 ) return 0;
    
        u128 x = amount * this->mScale * this->mPercent;
        u128 y = this->mScale * this->mScale;
        return ( u64 )(( x / y ) + (( x % y ) ? 1 : 0 ));
    }

    //----------------------------------------------------------------//
    FeePercent () :
        mScale ( 0 ),
        mPercent ( 0 ) {
    }
    
    //----------------------------------------------------------------//
    FeePercent ( u64 scale, u64 percent ) :
        mScale ( scale ),
        mPercent ( percent ) {
    }
};

//================================================================//
// FeeProfile
//================================================================//
class FeeProfile :
    public AbstractSerializable {
public:

    FeePercent  mProfitShare;           // percent of the gratuity offered to miner (in fixed point)
    FeePercent  mTransferTax;           // percent ot VOL transferred between accounts (in fixed point)
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "profitShare",       this->mProfitShare );
        serializer.serialize ( "transferTax",       this->mTransferTax );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "profitShare",       this->mProfitShare );
        serializer.serialize ( "transferTax",       this->mTransferTax );
    }

    //----------------------------------------------------------------//
    bool checkProfitShare ( u64 amount, u64 share ) const {
    
        return this->mProfitShare.check ( amount, share );
    }
    
    //----------------------------------------------------------------//
    bool checkTransferTax ( u64 amount, u64 share ) const {
    
        return this->mTransferTax.check ( amount, share );
    }
};

//================================================================//
// FeeSchedule
//================================================================//
class FeeSchedule :
    public AbstractSerializable {
public:
    
    u64                                         mFixedMiningReward;
    FeePercent                                  mScaledMiningReward;
    FeePercent                                  mMiningTax;
    
    FeeProfile                                  mDefaultProfile;
    SerializableMap < string, FeeProfile >      mTransactionProfiles;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {

        this->mFixedMiningReward = 0;

        serializer.serialize ( "fixedMiningReward",     this->mFixedMiningReward );
        serializer.serialize ( "scaledMiningReward",    this->mScaledMiningReward );
        serializer.serialize ( "miningTax",             this->mMiningTax );
    
        serializer.serialize ( "defaultProfile",        this->mDefaultProfile );
        serializer.serialize ( "transactionProfiles",   this->mTransactionProfiles );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "fixedMiningReward",     this->mFixedMiningReward );
        serializer.serialize ( "scaledMiningReward",    this->mScaledMiningReward );
        serializer.serialize ( "miningTax",             this->mMiningTax );
        
        serializer.serialize ( "defaultProfile",        this->mDefaultProfile );
        serializer.serialize ( "transactionProfiles",   this->mTransactionProfiles );
    }
    
    //----------------------------------------------------------------//
    u64 calculateMiningReward ( u64 rewardPool ) const {
    
        u64 maxReward = this->mFixedMiningReward + this->mScaledMiningReward.computeAndRoundUp ( rewardPool );
        return maxReward <= rewardPool ? maxReward : rewardPool;
    }
    
    //----------------------------------------------------------------//
    u64 calculateMiningRewardTax ( u64 miningReward ) const {
    
        return this->mMiningTax.computeAndRoundDown ( miningReward );
    }
    
    //----------------------------------------------------------------//
    FeeProfile getFeeProfile ( string feeType ) {
        
        SerializableMap < string, FeeProfile >::const_iterator profileIt = this->mTransactionProfiles.find ( feeType );
        if ( profileIt != this->mTransactionProfiles.cend ()) return profileIt->second;
        return this->mDefaultProfile;
    }
    
    //----------------------------------------------------------------//
    bool hasMiningReward () const {
    
        return (( this->mFixedMiningReward > 0  ) || this->mScaledMiningReward );
    }
};

} // namespace Volition
#endif
