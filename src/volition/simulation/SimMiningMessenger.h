// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATION_SIMMININGMESSENGER_H
#define VOLITION_SIMULATION_SIMMININGMESSENGER_H

#include <volition/common.h>
#include <volition/simulation/SimMiningNetwork.h>

namespace Volition {
namespace Simulation {

//================================================================//
// SimMiningMessenger
//================================================================//
class SimMiningMessenger :
    public virtual AbstractMiningMessenger {
protected:

    shared_ptr < SimMiningNetwork >    mNetwork;

    //----------------------------------------------------------------//
    bool AbstractMiningMessenger_isFull ( MiningMessengerRequest::Type requestType ) const override {
    
        UNUSED ( requestType );
        return true;
    }

    //----------------------------------------------------------------//
    void AbstractMiningMessenger_sendRequest ( const MiningMessengerRequest& request ) override {
    
        assert ( this->mNetwork );
        this->mNetwork->enqueueRequest ( this, request );
    }

public:

    SET ( shared_ptr < SimMiningNetwork >,      Network,        mNetwork )

    //----------------------------------------------------------------//
    SimMiningMessenger ( shared_ptr < SimMiningNetwork > network ) :
        mNetwork ( network ) {
    }
    
    //----------------------------------------------------------------//
    ~SimMiningMessenger () {
    }
};

} // namespace Simulation
} // namespace Volition
#endif
