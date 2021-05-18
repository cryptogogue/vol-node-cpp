// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MONETARYPOLICY_H
#define VOLITION_MONETARYPOLICY_H

#include <volition/common.h>
#include <volition/FixedPoint.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// MonetaryPolicy
//================================================================//
class MonetaryPolicy :
    public AbstractSerializable {
public:
    
    u64                     mFixedMiningReward;
    FixedPoint              mScaledMiningReward;
    FixedPoint              mMiningTax;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {

        this->mFixedMiningReward = 0;

        serializer.serialize ( "fixedMiningReward",     this->mFixedMiningReward );
        serializer.serialize ( "scaledMiningReward",    this->mScaledMiningReward );
        serializer.serialize ( "miningTax",             this->mMiningTax );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "fixedMiningReward",     this->mFixedMiningReward );
        serializer.serialize ( "scaledMiningReward",    this->mScaledMiningReward );
        serializer.serialize ( "miningTax",             this->mMiningTax );
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
    bool hasMiningReward () const {
    
        return (( this->mFixedMiningReward > 0  ) || this->mScaledMiningReward );
    }
};

} // namespace Volition
#endif
