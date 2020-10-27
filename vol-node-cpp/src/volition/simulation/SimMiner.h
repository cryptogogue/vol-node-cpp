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
    
    //----------------------------------------------------------------//
    shared_ptr < Block >    replaceBlock            ( shared_ptr < const Block > oldBlock, string charmHex );

public:

    //----------------------------------------------------------------//
    void                    extendChain             ( string charmHex, time_t time = 0 );
    void                    rewindChain             ( size_t height );
    void                    setCharm                ( size_t height, string charmHex = "" );
    void                    scrambleRemotes         ();
                            SimMiner                ();
                            ~SimMiner               ();
};

} // namespace Simulation
} // namespace Volition
#endif
