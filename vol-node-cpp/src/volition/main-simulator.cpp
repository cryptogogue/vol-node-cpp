// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <padamose/padamose.h>
#include <volition/Block.h>
#include <volition/Digest.h>
#include <volition/FileSys.h>
#include <volition/Miner.h>
#include <volition/MinerAPIFactory.h>
#include <volition/RouteTable.h>
#include <volition/simulation/SimulatorActivity.h>
#include <volition/version.h>

using namespace Volition;
using namespace Simulation;

const size_t TOTAL_MINERS   = 16;
const int BASE_PORT         = 9090;

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
        simulator.initialize ( TOTAL_MINERS, BASE_PORT );
        
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
