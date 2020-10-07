// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATION_SIMMININGMESSENGER_H
#define VOLITION_SIMULATION_SIMMININGMESSENGER_H

#include <volition/common.h>
#include <volition/AbstractMiningMessenger.h>
#include <volition/Miner.h>

namespace Volition {
namespace Simulation {

//================================================================//
// SimMiningMessengerConstraint
//================================================================//
class SimMiningMessengerConstraint {
public:

    friend class SimMiningMessenger;

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
    SimMiningMessengerConstraint () :
        mMode ( CONSTRAINT_NONE ),
        mProbability ( 0 ) {
    }
};

//================================================================//
// SimMiningMessenger
//================================================================//
class SimMiningMessenger :
    public virtual AbstractMiningMessenger {
protected:

    typedef list < SimMiningMessengerConstraint> ConstraintList;

    static const size_t HEADER_BATCH_SIZE   = 32;

    mt19937                                             mPRNG;
    uniform_real_distribution < double >                mUniformDistribution;

    map < string, shared_ptr < Miner >>                 mMiners;
    map < string, ConstraintList >                      mConstraintLists;
    vector < ConstraintList* >                          mConstraintListsByIndex;
    
    list < shared_ptr < MiningMessengerRequest >>       mTasks;

    //----------------------------------------------------------------//
    void        dispatchBlock               ( const MiningMessengerRequest& request, shared_ptr < const Block > block );
    void        dispatchHeaders             ( const MiningMessengerRequest& request, list < shared_ptr < const BlockHeader >> headers );
    void        handleTask                  ( const MiningMessengerRequest& task );
    void        pushConstraint              ( const SimMiningMessengerConstraint& constraint, size_t base, size_t top );
    void        pushConstraint              ( SimMiningMessengerConstraint::Mode mode, double probability, size_t base, size_t top );
    double      random                      ();

    //----------------------------------------------------------------//
    void        AbstractMiningMessenger_request         ( const MiningMessengerRequest& request ) override;

public:

    //----------------------------------------------------------------//
    void        clearConstraint             ( size_t base, size_t top = 0 );
    void        pushConstraintDropBlock     ( double probability, size_t base, size_t top = 0 );
    void        pushConstraintDropHeader    ( double probability, size_t base, size_t top = 0 );
                SimMiningMessenger          ();
                ~SimMiningMessenger         ();
    void        setMiners                   ( vector < shared_ptr < Miner >> miners );
    void        updateAndDispatch           ();
};

} // namespace Simulation
} // namespace Volition
#endif
