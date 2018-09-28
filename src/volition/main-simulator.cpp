// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/simulator/Scenario.h>

//================================================================//
// ConsensusApp
//================================================================//
class ConsensusApp :
    public Poco::Util::Application {
public:

    //----------------------------------------------------------------//
    int main ( const vector < string > &args ) override {
    
        Volition::Simulator::SmallScenario scenario;
        scenario.run ();
        return EXIT_OK;
    }
};

POCO_APP_MAIN ( ConsensusApp );
