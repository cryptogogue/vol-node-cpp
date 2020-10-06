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
#include <volition/simulation/SimulatorActivity.h>
#include <volition/version.h>

using namespace Volition;
using namespace Simulation;

const int BASE_PORT         = 9090;

//================================================================//
// SimpleScenario
//================================================================//
class SimpleScenario :
    public AbstractScenario {
protected:

    SCENARIO_BASE_PORT ( BASE_PORT )
    SCENARIO_REPORT_MODE ( Simulator::REPORT_ALL_MINERS )
    SCENARIO_SIZE ( 16 )
    
    //----------------------------------------------------------------//
    void AbstractScenario_control ( Simulator& simulator, SimMiningMessenger& messenger, size_t step ) override {
        UNUSED ( simulator );
        UNUSED ( messenger );
        UNUSED ( step );
    }
    
    //----------------------------------------------------------------//
    void AbstractScenario_setup ( Simulator& simulator ) override {
        UNUSED ( simulator );
    }
};

//================================================================//
// MixedScenario
//================================================================//
class MixedScenario :
    public AbstractScenario {
protected:

    SCENARIO_BASE_PORT ( BASE_PORT )
    SCENARIO_REPORT_MODE ( Simulator::REPORT_ALL_MINERS )
    SCENARIO_SIZE ( 16 )
    
    //----------------------------------------------------------------//
    void AbstractScenario_control ( Simulator& simulator, SimMiningMessenger& messenger, size_t step ) override {
        UNUSED ( simulator );
        UNUSED ( messenger );
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
};

//================================================================//
// RandomDropScenario
//================================================================//
class RandomDropScenario :
    public AbstractScenario {
protected:

    SCENARIO_BASE_PORT ( BASE_PORT )
    SCENARIO_REPORT_MODE ( Simulator::REPORT_ALL_MINERS )
    SCENARIO_SIZE ( 16 )
    
    //----------------------------------------------------------------//
    void AbstractScenario_control ( Simulator& simulator, SimMiningMessenger& messenger, size_t step ) override {
        UNUSED ( simulator );
        UNUSED ( messenger );
        UNUSED ( step );
        
        switch ( step ) {
            case 0:
                messenger.pushConstraintDropBlock ( 0.875, 0, 16 );
                break;
            
            case 64:
                simulator.getSimMiner ( 0 )->setCharm ( 8 );
//                simulator.pause ();
                break;
        }
    }
};

//================================================================//
// RewriteScenario
//================================================================//
class RewriteScenario :
    public AbstractScenario {
protected:

    SCENARIO_BASE_PORT ( BASE_PORT )
    SCENARIO_REPORT_MODE ( Simulator::REPORT_ALL_MINERS )
    SCENARIO_SIZE ( 16 )
    
    int     mCounter;
    
    //----------------------------------------------------------------//
    void AbstractScenario_control ( Simulator& simulator, SimMiningMessenger& messenger, size_t step ) override {
        UNUSED ( simulator );
        UNUSED ( messenger );
        UNUSED ( step );
        
        if ( step == 0 ) {
            simulator.setTimeStep ( 1 );
            simulator.setRewriteWindow ( 0, 16, 1 );
        }
        
        shared_ptr < SimMiner > simMiner = simulator.getSimMiner ( 0 );
        if (( **simMiner->getBestBranch ()).getHeight () >= 16 ) {
        
            BlockTreeNode::ConstPtr prevBranch = simMiner->getBestBranch ();
        
            simMiner->setCharm ( 8, Format::write ( "%06x", this->mCounter ));
            this->mCounter--;
        }
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

    SCENARIO_BASE_PORT ( BASE_PORT )
    SCENARIO_REPORT_MODE ( Simulator::REPORT_ALL_MINERS )
    SCENARIO_SIZE ( 16 )
        
    //----------------------------------------------------------------//
    void AbstractScenario_control ( Simulator& simulator, SimMiningMessenger& messenger, size_t step ) override {
        UNUSED ( simulator );
        UNUSED ( messenger );
        UNUSED ( step );
        
//        if ( this->random () < 0.0625 ) {
        if ( this->random () < 0.125 ) {
        
            shared_ptr < SimMiner > simMiner = simulator.getSimMiner ( this->random ( 0, 15 ));
            simMiner->scrambleRemotes ();
            simMiner->rewindChain ( this->random ( 0, ( **simMiner->getBestBranch ()).getHeight ()));
        }
    }
};

#define THE_SCENARO ScrambleScenario

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
        
        Poco::ThreadPool threadPool;
        
        shared_ptr < Poco::Net::HTTPServer > server = make_shared < Poco::Net::HTTPServer >(
            new Volition::MinerAPIFactory ( simulator.getMiners ()),
            threadPool,
            Poco::Net::ServerSocket (( Poco::UInt16 )BASE_PORT ),
            new Poco::Net::HTTPServerParams ()
        );
        server->start ();

        simulator.start ();

        // nasty little hack. POCO considers the set breakpoint signal to be a termination event.
        // need to find out how to stop POCO from doing this. in the meantime, this hack.
        #ifdef _DEBUG
            Poco::Event shutdownEvent;
            shutdownEvent.wait ();
        #else
            this->waitForTerminationRequest ();  // wait for CTRL-C or kill
        #endif

        simulator.shutdown ();

        server->stop ();
        
        threadPool.stopAll ();

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

    SimulatorApp app;
    return app.run ( argc, argv );
}
