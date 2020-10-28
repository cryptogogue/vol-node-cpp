// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMMINER_H
#define VOLITION_SIMMINER_H

#include <volition/Miner.h>

namespace Volition {
namespace Simulation {

//================================================================//
// SimMiner
//================================================================//
class SimMiner :
    public Miner {
protected:
    
    friend class AbstractScenario;
    friend class Simulator;
    friend class SimMiningMessenger;
    
    bool            mActive;
    size_t          mInterval;
    bool            mIsGenesisMiner;
    
    //----------------------------------------------------------------//
    shared_ptr < Block >    replaceBlock            ( shared_ptr < const Block > oldBlock, string charmHex );

public:

    //----------------------------------------------------------------//
    void                    extendChain             ( string charmHex, time_t time = 0 );
    void                    rewindChain             ( size_t height );
    void                    setActive               ( bool active );
    void                    setCharm                ( size_t height, string charmHex = "" );
    void                    scrambleRemotes         ();
                            SimMiner                ( bool isGenesisMiner = true );
                            ~SimMiner               ();
};

} // namespace Simulation
} // namespace Volition
#endif
