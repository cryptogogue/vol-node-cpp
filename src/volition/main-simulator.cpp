// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <padamose/padamose.h>
#include <volition/Block.h>
#include <volition/Digest.h>
#include <volition/FileSys.h>
#include <volition/Miner.h>
#include <volition/MinerAPIFactory.h>
#include <volition/RouteTable.h>
#include <volition/simulation/AbstractScenario.h>
#include <volition/simulation/SimTransaction.h>
#include <volition/simulation/SimulatorActivity.h>
#include <volition/UnsecureRandom.h>
#include <volition/version.h>

using namespace Volition;
using namespace Simulation;

const int BASE_PORT         = 9090;

#define THE_SCENARO SimpleScenario

//================================================================//
// CatchUpScenario
//================================================================//
class CatchUpScenario :
    public AbstractScenario {
protected:
    
    //----------------------------------------------------------------//
    void AbstractScenario_control ( Simulator& simulator, SimMiningNetwork& network, size_t step ) override {
        UNUSED ( network );
        
        switch ( step ) {
            case 64:
                simulator.setActive ( 2, 3, true );
                break;
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractScenario_setup ( Simulator& simulator ) override {
        
        simulator.initializeMiners ( 3, 0, BASE_PORT );
        simulator.setActive ( 2, 3, false );
        simulator.initializeGenesis ();
        simulator.setReportMode ( Simulator::REPORT_ALL_MINERS );
    }
};

//================================================================//
// MinimalScenario
//================================================================//
class MinimalScenario :
    public AbstractScenario {
protected:
    
    //----------------------------------------------------------------//
    void AbstractScenario_control ( Simulator& simulator, SimMiningNetwork& network, size_t step ) override {
        UNUSED ( simulator );
        UNUSED ( network );
        UNUSED ( step );
    }
    
    //----------------------------------------------------------------//
    void AbstractScenario_setup ( Simulator& simulator ) override {
        
        simulator.initializeMiners ( 2, 0, BASE_PORT );
        simulator.initializeGenesis ();
        simulator.setReportMode ( Simulator::REPORT_ALL_MINERS_VERBOSE );
    }
};

//================================================================//
// SimpleScenario
//================================================================//
class SimpleScenario :
    public AbstractScenario {
protected:
    
    //----------------------------------------------------------------//
    void AbstractScenario_control ( Simulator& simulator, SimMiningNetwork& network, size_t step ) override {
        UNUSED ( simulator );
        UNUSED ( network );
        UNUSED ( step );
    }
    
    //----------------------------------------------------------------//
    void AbstractScenario_setup ( Simulator& simulator ) override {
        
        simulator.initializeMiners ( 16, 0, BASE_PORT );
        simulator.initializeGenesis ();
        simulator.setReportMode ( Simulator::REPORT_ALL_MINERS );
    }
};

//================================================================//
// MinerScenario
//================================================================//
class MinerScenario :
    public AbstractScenario {
protected:
    
    //----------------------------------------------------------------//
    void AbstractScenario_control ( Simulator& simulator, SimMiningNetwork& network, size_t step ) override {
        UNUSED ( simulator );
        UNUSED ( network );
        UNUSED ( step );
        
        switch ( step ) {

            case 0:
            
                // add 9091 as a miner
                simulator.setActive ( 1, 2, true );
                simulator.getSimMiner ( 0 )->getTransactionQueue ().pushTransaction (
                    SimTransaction::makeTransaction (
                        SimTransaction::makeBody_RegisterMiner ( *simulator.getSimMiner ( 1 ), simulator.getSimMiner ( 1 )->getURL ()),
                        "0e449540-5c04-4c2b-a437-dcc75db54de8",
                        "9091"
                    )
                );
                break;
            
            case 2:

                // 9092 and 9093 wake up
                simulator.setActive ( 2, 4, true );
                simulator.getSimMiner ( 2 )->affirmRemoteMiner ( simulator.getSimMiner ( 1 )->getURL ());
                simulator.getSimMiner ( 3 )->affirmRemoteMiner ( simulator.getSimMiner ( 1 )->getURL ());
                break;

            case 4:

                // 9090 goes away
                simulator.setActive ( 0, 1, false );

            case 6:

                // add 9092, 9093 as miners
                simulator.getSimMiner ( 1 )->getTransactionQueue ().pushTransaction (
                    SimTransaction::makeTransaction (
                        SimTransaction::makeBody_RegisterMiner ( *simulator.getSimMiner ( 2 ), simulator.getSimMiner ( 2 )->getURL ()),
                        "12ad8de5-0aa1-4fbf-a6fe-bdce7640aed3",
                        "9092"
                    )
                );

                simulator.getSimMiner ( 1 )->getTransactionQueue ().pushTransaction (
                    SimTransaction::makeTransaction (
                        SimTransaction::makeBody_RegisterMiner ( *simulator.getSimMiner ( 3 ), simulator.getSimMiner ( 3 )->getURL ()),
                        "333a6f1b-cdf3-4b1d-8979-898d99d22f5c",
                        "9093"
                    )
                );
                break;

            case 8:

                // wake up 9094 - 9097
                simulator.setActive ( 4, 8, true );

                simulator.getSimMiner ( 4 )->affirmRemoteMiner ( simulator.getSimMiner ( 1 )->getURL ());
                simulator.getSimMiner ( 5 )->affirmRemoteMiner ( simulator.getSimMiner ( 1 )->getURL ());
                simulator.getSimMiner ( 6 )->affirmRemoteMiner ( simulator.getSimMiner ( 1 )->getURL ());
                simulator.getSimMiner ( 7 )->affirmRemoteMiner ( simulator.getSimMiner ( 1 )->getURL ());

                break;

            case 10:

                // add 9094 - 9097 as miners
                simulator.getSimMiner ( 1 )->getTransactionQueue ().pushTransaction (
                    SimTransaction::makeTransaction (
                        SimTransaction::makeBody_RegisterMiner ( *simulator.getSimMiner ( 4 ), simulator.getSimMiner ( 4 )->getURL ()),
                        "8f460ff6-a290-44c7-98d3-a678449a613b",
                        "9094"
                    )
                );

                simulator.getSimMiner ( 1 )->getTransactionQueue ().pushTransaction (
                    SimTransaction::makeTransaction (
                        SimTransaction::makeBody_RegisterMiner ( *simulator.getSimMiner ( 5 ), simulator.getSimMiner ( 5 )->getURL ()),
                        "1d69ad2a-8ee1-4f35-968f-a4ce725c8dea",
                        "9095"
                    )
                );

                simulator.getSimMiner ( 1 )->getTransactionQueue ().pushTransaction (
                    SimTransaction::makeTransaction (
                        SimTransaction::makeBody_RegisterMiner ( *simulator.getSimMiner ( 6 ), simulator.getSimMiner ( 6 )->getURL ()),
                        "a9f4409e-7a1b-48ef-8fcf-b58070a2ee9a",
                        "9096"
                    )
                );

                simulator.getSimMiner ( 1 )->getTransactionQueue ().pushTransaction (
                    SimTransaction::makeTransaction (
                        SimTransaction::makeBody_RegisterMiner ( *simulator.getSimMiner ( 7 ), simulator.getSimMiner ( 7 )->getURL ()),
                        "356cff67-a911-4602-8950-a667fd2fd040",
                        "9097"
                    )
                );

                break;
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractScenario_setup ( Simulator& simulator ) override {
        
        simulator.initializeMiners ( 8, 7, BASE_PORT );
        simulator.setActive ( 1, 8, false );
        simulator.initializeGenesis ();
        simulator.setReportMode ( Simulator::REPORT_ALL_MINERS );
    }
};

//================================================================//
// MixedScenario
//================================================================//
class MixedScenario :
    public AbstractScenario {
protected:
    
    //----------------------------------------------------------------//
    void AbstractScenario_control ( Simulator& simulator, SimMiningNetwork& network, size_t step ) override {
        UNUSED ( simulator );
        UNUSED ( network );
        UNUSED ( step );
        
        switch ( step ) {
            case 0:
                simulator.setInterval ( 0, 8,   1 );
                simulator.setInterval ( 8, 16,  4 );
                break;
            
            case 32:
                simulator.setInterval ( 0, 8,   4 );
                simulator.setInterval ( 8, 16,  1 );
                break;
            
            case 64:
//                simulator.pause ();
                break;
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractScenario_setup ( Simulator& simulator ) override {
        
        simulator.initializeMiners ( 16, 0, BASE_PORT );
        simulator.initializeGenesis ();
        simulator.setReportMode ( Simulator::REPORT_ALL_MINERS );
    }
};

//================================================================//
// RandomDropScenario
//================================================================//
class RandomDropScenario :
    public AbstractScenario {
protected:
    
    //----------------------------------------------------------------//
    void AbstractScenario_control ( Simulator& simulator, SimMiningNetwork& network, size_t step ) override {
        UNUSED ( simulator );
        UNUSED ( network );
        UNUSED ( step );
        
        switch ( step ) {
            case 0:
                network.pushConstraintDropBlock ( 0.875, 0, 16 );
                break;
            
            case 64:
                simulator.getSimMiner ( 0 )->setCharm ( 8 );
//                simulator.pause ();
                break;
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractScenario_setup ( Simulator& simulator ) override {
        
        simulator.initializeMiners ( 16, 0, BASE_PORT );
        simulator.initializeGenesis ();
        simulator.setReportMode ( Simulator::REPORT_ALL_MINERS );
    }
};

//================================================================//
// RewriteScenario
//================================================================//
class RewriteScenario :
    public AbstractScenario {
protected:
    
    int     mCounter;
    
    //----------------------------------------------------------------//
    void AbstractScenario_control ( Simulator& simulator, SimMiningNetwork& network, size_t step ) override {
        UNUSED ( simulator );
        UNUSED ( network );
        UNUSED ( step );
        
        if ( step == 0 ) {
            simulator.setTimeStepInSeconds ( 600 );
        }
        
        shared_ptr < SimMiner > simMiner = simulator.getSimMiner ( 0 );
        if ( simMiner->getLedgerTag ().getHeight () >= 16 ) {
        
//            BlockTreeNode::ConstPtr prevBranch = simMiner->getLedgerTag ();
        
            simMiner->setCharm ( 8, Format::write ( "%06x", this->mCounter ));
            this->mCounter--;
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractScenario_setup ( Simulator& simulator ) override {
        
        simulator.initializeMiners ( 16, 0, BASE_PORT );
        simulator.initializeGenesis ();
        simulator.setReportMode ( Simulator::REPORT_ALL_MINERS );
    }

public:

    //----------------------------------------------------------------//
    RewriteScenario () :
        mCounter ( 0x0000ff ) {
    }
};

//================================================================//
// ScrambleScenario
//================================================================//
class ScrambleScenario :
    public AbstractScenario {
protected:
        
    //----------------------------------------------------------------//
    void AbstractScenario_control ( Simulator& simulator, SimMiningNetwork& network, size_t step ) override {
        UNUSED ( simulator );
        UNUSED ( network );
        UNUSED ( step );
        
//        if ( this->random () < 0.0625 ) {
        if ( UnsecureRandom::get ().random () < 0.25 ) {
        
            shared_ptr < SimMiner > simMiner = simulator.getSimMiner ( UnsecureRandom::get ().random ( 0, 15 ));
            simMiner->scrambleRemotes ();
            simMiner->rewindChain ( UnsecureRandom::get ().random ( 0, simMiner->getLedgerTag ().getHeight ()));
        }
    }
    
    
    //----------------------------------------------------------------//
    void AbstractScenario_setup ( Simulator& simulator ) override {
        
        simulator.initializeMiners ( 16, 0, BASE_PORT );
        simulator.initializeGenesis ();
        simulator.setReportMode ( Simulator::REPORT_ALL_MINERS );
    }
};

//================================================================//
// SimulatorApp
//================================================================//
class SimulatorApp :
    public Poco::Util::ServerApplication {
public:

    //----------------------------------------------------------------//
    int main ( const vector < string > &args ) override {
        UNUSED ( args );
        
        SimulatorActivity simulator;
        simulator.initialize ( make_shared < THE_SCENARO >());

        simulator.start ();

        #ifdef _DEBUG
            Poco::Event shutdownEvent;
            shutdownEvent.wait ();
        #else
            this->waitForTerminationRequest ();  // wait for CTRL-C or kill
        #endif

        simulator.shutdown ();

        return EXIT_OK;
    }
};

//================================================================//
// main
//================================================================//

//----------------------------------------------------------------//
int main ( int argc, char** argv ) {

    Lognosis::setFilter ( PDM_FILTER_ROOT, Lognosis::OFF );
    Lognosis::init ( argc, argv );
    LOG_F ( INFO, "Hello from main.cpp!" );

    Lognosis::setFilter ( PDM_FILTER_ROOT,                      Lognosis::OFF );
    Lognosis::setFilter ( PDM_FILTER_SQLITE,                    Lognosis::OFF );
    Lognosis::setFilter ( PDM_FILTER_SQLSTORE,                  Lognosis::OFF );
    
    Lognosis::setFilter ( VOL_FILTER_APP,                       Lognosis::OFF );
    Lognosis::setFilter ( VOL_FILTER_BLOCK,                     Lognosis::OFF );
    Lognosis::setFilter ( VOL_FILTER_CONSENSUS,                 Lognosis::OFF );
    
    Lognosis::setFilter ( VOL_FILTER_HTTP,                      Lognosis::OFF );
    Lognosis::setFilter ( VOL_FILTER_JSON,                      Lognosis::OFF );
    Lognosis::setFilter ( VOL_FILTER_LEDGER,                    Lognosis::OFF );
    Lognosis::setFilter ( VOL_FILTER_LUA,                       Lognosis::OFF );
    
//    Lognosis::setFilter ( VOL_FILTER_MINING_REPORT,             Lognosis::OFF );
    Lognosis::setFilter ( VOL_FILTER_MINING_SEARCH_REPORT,      Lognosis::OFF );
    Lognosis::setFilter ( VOL_FILTER_QUEUE,                     Lognosis::OFF );

    Lognosis::init ( argc, argv );

    SimulatorApp app;
    return app.run ( argc, argv );
}
