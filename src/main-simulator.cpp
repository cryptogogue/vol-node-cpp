// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "analysis.h"
#include "context.h"
#include "cohort.h"
#include "scenario.h"

#include "Block.h"

//================================================================//
// ConsensusApp
//================================================================//
class ConsensusApp :
    public Poco::Util::Application {
public:

    //----------------------------------------------------------------//
    int main ( const vector < string > &args ) {
    
        Block block;
        block.sign ();
    
        FastGangScenario scenario;
        scenario.Run ();
        return EXIT_OK;
    }
};

POCO_APP_MAIN ( ConsensusApp );
