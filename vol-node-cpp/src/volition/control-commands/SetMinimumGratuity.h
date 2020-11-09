// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_COMMANDS_SET_MINIMUM_GRATUITY_H
#define VOLITION_COMMANDS_SET_MINIMUM_GRATUITY_H

#include <volition/common.h>
#include <volition/AbstractControlCommandBody.h>
#include <volition/Digest.h>
#include <volition/Miner.h>
#include <volition/Schema.h>

namespace Volition {
namespace Commands {

//================================================================//
// SetMinimumGratuity
//================================================================//
class SetMinimumGratuity :
    public AbstractControlCommandBody {
public:

    COMMAND_TYPE ( "SET_MINIMUM_GRATUITY" )
    COMMAND_CONTROL_LEVEL ( Miner::CONTROL_CONFIG )

    u64         mMinimum;

    //----------------------------------------------------------------//
    LedgerResult AbstractControlCommandBody_execute ( Miner& miner  ) const override {
        
        miner.setMinimumGratuity ( this->mMinimum );
        return true;
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractControlCommandBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "minimum",       this->mMinimum );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractControlCommandBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "minimum",       this->mMinimum );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
