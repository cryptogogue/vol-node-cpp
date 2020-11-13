// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATION_SIMMININGNETWORK_H
#define VOLITION_SIMULATION_SIMMININGNETWORK_H

#include <volition/common.h>
#include <volition/AbstractMiningMessenger.h>
#include <volition/Miner.h>
#include <volition/simulation/SimMiner.h>

namespace Volition {
namespace Simulation {

//================================================================//
// SimMiningNetworkConstraint
//================================================================//
class SimMiningNetworkConstraint {
public:

    friend class SimMiningNetwork;

    enum Mode {
        CONSTRAINT_NONE,
        CONSTRAINT_DELAY,
        CONSTRAINT_DROP_BLOCK,
        CONSTRAINT_DROP_HEADER,
        CONSTRAINT_DELAY_AND_DROP,
    };

private:

    Mode        mMode;
    double      mProbability;

public:

    //----------------------------------------------------------------//
    SimMiningNetworkConstraint () :
        mMode ( CONSTRAINT_NONE ),
        mProbability ( 0 ) {
    }
};

//================================================================//
// SimMiningNetwork
//================================================================//
class SimMiningNetwork {
protected:

    typedef list < SimMiningNetworkConstraint> ConstraintList;

    static const size_t HEADER_BATCH_SIZE       = 4;
    static const size_t MINER_URL_BATCH_SIZE    = 2;

    map < string, shared_ptr < SimMiner >>              mMinersByURL;
    map < string, ConstraintList >                      mConstraintLists;
    vector < ConstraintList* >                          mConstraintListsByIndex;
    
    list < pair < AbstractMiningMessenger*, MiningMessengerRequest >> mQueue;
    
    //----------------------------------------------------------------//
    shared_ptr < SimMiner >     getMiner                    ( const MiningMessengerRequest& request );
    const ConstraintList&       getMinerConstraints         ( const MiningMessengerRequest& request );
    void                        handleRequest               ( AbstractMiningMessenger* client, const MiningMessengerRequest& request );
    void                        pushConstraint              ( const SimMiningNetworkConstraint& constraint, size_t base, size_t top );
    void                        pushConstraint              ( SimMiningNetworkConstraint::Mode mode, double probability, size_t base, size_t top );

public:

    //----------------------------------------------------------------//
    void            clearConstraint                 ( size_t base, size_t top = 0 );
    void            enqueueRequest                  ( AbstractMiningMessenger* messenger, const MiningMessengerRequest& request );
    void            pushConstraintDropBlock         ( double probability, size_t base, size_t top = 0 );
    void            pushConstraintDropHeader        ( double probability, size_t base, size_t top = 0 );
                    SimMiningNetwork       ();
                    ~SimMiningNetwork      ();
    void            setMiners                       ( vector < shared_ptr < Miner >> miners );
    void            updateAndDispatch               ();
};

} // namespace Simulation
} // namespace Volition
#endif
