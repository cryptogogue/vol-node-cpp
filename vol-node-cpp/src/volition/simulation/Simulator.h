// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATION_SIMULATOR_H
#define VOLITION_SIMULATION_SIMULATOR_H

#include <volition/common.h>
#include <volition/Block.h>
#include <volition/InMemoryBlockTree.h>
#include <volition/Ledger.h>
#include <volition/simulation/Analysis.h>
#include <volition/simulation/SimMiner.h>
#include <volition/simulation/SimMiningMessenger.h>
#include <volition/Transaction.h>

namespace Volition {
namespace Simulation {

class AbstractScenario;

//================================================================//
// Simulator
//================================================================//
class Simulator {
public:

    typedef vector < shared_ptr < Miner >> Miners;

    enum ReportMode {
        REPORT_SUMMARY,
        REPORT_SINGLE_MINER,
        REPORT_SINGLE_MINER_VS_OPTIMAL,
        REPORT_ALL_MINERS,
    };

protected:
    
    shared_ptr < AbstractScenario >     mScenario;
    ReportMode                          mReportMode;
    
    Analysis                            mAnalysis;
    
    size_t                              mBasePort;
    Miners                              mMiners;
    Poco::Event                         mShutdownEvent;
    
    InMemoryBlockTree                           mOptimal;
    BlockTreeTag                        mOptimalTag;

    bool                                mIsPaused;
    size_t                              mStepCount;
    time_t                              mNow;
    time_t                              mTimeStep;

    shared_ptr < SimMiningNetwork >     mNetwork;

    //----------------------------------------------------------------//
    void                                extendOptimal           ( size_t height );
    void                                prepare                 ();
    void                                step                    ();

public:

    //----------------------------------------------------------------//
    const Miners&                       getMiners               ();
    shared_ptr < SimMiner >             getSimMiner             ( size_t idx );
    void                                initialize              ( size_t totalMiners, size_t deferredMiners = 0, size_t basePort = 9090 );
    void                                initialize              ( shared_ptr < AbstractScenario > scenario );
    void                                pause                   ( bool pause = true );
    void                                report                  ();
    void                                setActive               ( size_t base, size_t top, bool active );
    void                                setInterval             ( size_t base, size_t top, size_t interval );
    void                                setMinerKey             ( size_t idx, const CryptoKeyPair& key );
    void                                setMinerKey             ( size_t idx, string pem );
    void                                setReportMode           ( ReportMode reportMode );
    void                                setTimeStepInSeconds    ( time_t seconds );
                                        Simulator               ();
                                        ~Simulator              ();
};

} // namespace Simulation
} // namespace Volition
#endif
