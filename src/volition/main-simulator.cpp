// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <simulator/Scenario.h>

//================================================================//
// ConsensusApp
//================================================================//
class ConsensusApp :
    public Poco::Util::Application {
public:

    //----------------------------------------------------------------//
    int main ( const vector < string > &args ) override {
    
        Volition::Simulator::SimpleScenario scenario;
        scenario.Run ();
        return EXIT_OK;
    }
};

POCO_APP_MAIN ( ConsensusApp );
