// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/simulation/SimulatorActivity.h>

namespace Volition {
namespace Simulation {

//================================================================//
// SimulatorActivity
//================================================================//

//----------------------------------------------------------------//
void SimulatorActivity::runActivity () {

    while ( !this->isStopped ()) {
        
        this->step ();
        Poco::Thread::sleep ( 10 );
    }
}


//----------------------------------------------------------------//
SimulatorActivity::SimulatorActivity () :
    Poco::Activity < SimulatorActivity >( this, &SimulatorActivity::runActivity ) {
}

//----------------------------------------------------------------//
SimulatorActivity::~SimulatorActivity () {
}

//----------------------------------------------------------------//
void SimulatorActivity::shutdown () {

    if ( !this->isStopped ()) {
        this->stop ();
    }
    this->wait ();
}

} // namespace Simulator
} // namespace Volition
