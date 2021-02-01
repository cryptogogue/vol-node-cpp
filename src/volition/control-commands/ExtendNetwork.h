// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_COMMANDS_EXTEND_NETWORK_H
#define VOLITION_COMMANDS_EXTEND_NETWORK_H

#include <volition/common.h>
#include <volition/AbstractControlCommandBody.h>
#include <volition/Miner.h>

namespace Volition {
namespace Commands {

//================================================================//
// ExtendNetwork
//================================================================//
class ExtendNetwork :
    public AbstractControlCommandBody {
public:

    COMMAND_TYPE ( "EXTEND_NETWORK" )
    COMMAND_CONTROL_LEVEL ( Miner::CONTROL_CONFIG )

    string      mURL;

    //----------------------------------------------------------------//
    LedgerResult AbstractControlCommandBody_execute ( Miner& miner  ) const override {
    
        printf ( "Extend Newtork: %s\n", this->mURL.c_str ());
        miner.affirmRemoteMiner ( this->mURL );
        
        return true;
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractControlCommandBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "url",       this->mURL );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractControlCommandBody::AbstractSerializable_serializeTo ( serializer );
    
        serializer.serialize ( "url",       this->mURL );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
