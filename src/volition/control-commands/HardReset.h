// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_COMMANDS_PUBLISH_HARD_RESET_H
#define VOLITION_COMMANDS_PUBLISH_HARD_RESET_H

#include <volition/common.h>
#include <volition/AbstractControlCommandBody.h>
#include <volition/Digest.h>
#include <volition/Miner.h>
#include <volition/Schema.h>

namespace Volition {
namespace Commands {

//================================================================//
// HardReset
//================================================================//
class HardReset :
    public AbstractControlCommandBody {
public:

    COMMAND_TYPE ( "HARD_RESET" )
    COMMAND_CONTROL_LEVEL ( Miner::CONTROL_ADMIN )

    //----------------------------------------------------------------//
    LedgerResult AbstractControlCommandBody_execute ( Miner& miner  ) const override {
        
        printf ( "CONTROL: doing hard reset.\n" );
        
        miner.reset ();
        miner.shutdown ( true );
        
        return true;
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractControlCommandBody::AbstractSerializable_serializeFrom ( serializer );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractControlCommandBody::AbstractSerializable_serializeTo ( serializer );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
