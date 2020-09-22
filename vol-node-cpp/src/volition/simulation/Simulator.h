// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATION_SIMULATOR_H
#define VOLITION_SIMULATION_SIMULATOR_H

#include <volition/common.h>
#include <volition/Block.h>
#include <volition/BlockTree.h>
#include <volition/Chain.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>
#include <volition/simulation/Analysis.h>
#include <volition/simulation/SimMiningMessenger.h>
#include <volition/Transaction.h>

namespace Volition {
namespace Simulation {

//================================================================//
// Simulator
//================================================================//
class Simulator {
public:

    typedef vector < shared_ptr < Miner >> Miners;

protected:
    
    Analysis                            mAnalysis;
    
    Miners                              mMiners;
    shared_ptr < SimMiningMessenger >   mMessenger;
    Poco::Event                         mShutdownEvent;
    
    BlockTree                           mOptimal;
    BlockTreeNode::ConstPtr             mOptimalTag;

    //----------------------------------------------------------------//
    void            extendOptimal       ( size_t height );
    void            step                ();

public:

    //----------------------------------------------------------------//
    const Miners&   getMiners           ();
    void            initialize          ( size_t totalMiners, int basePort = 9090 );
                    Simulator           ();
                    ~Simulator          ();
};

} // namespace Simulation
} // namespace Volition
#endif
