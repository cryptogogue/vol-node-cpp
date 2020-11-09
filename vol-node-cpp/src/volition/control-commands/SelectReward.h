// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_COMMANDS_SELECT_REWARD_H
#define VOLITION_COMMANDS_SELECT_REWARD_H

#include <volition/common.h>
#include <volition/AbstractControlCommandBody.h>
#include <volition/Digest.h>
#include <volition/Miner.h>
#include <volition/Schema.h>

namespace Volition {
namespace Commands {

//================================================================//
// SelectReward
//================================================================//
class SelectReward :
    public AbstractControlCommandBody {
public:

    COMMAND_TYPE ( "SELECT_REWARD" )
    COMMAND_CONTROL_LEVEL ( Miner::CONTROL_CONFIG )

    string      mReward;

    //----------------------------------------------------------------//
    LedgerResult AbstractControlCommandBody_execute ( Miner& miner  ) const override {
        
        miner.setReward ( this->mReward );
        return true;
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractControlCommandBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "reward",    this->mReward );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractControlCommandBody::AbstractSerializable_serializeTo ( serializer );
    
        serializer.serialize ( "reward",    this->mReward );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
