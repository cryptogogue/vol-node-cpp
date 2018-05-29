//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "analysis.h"
#include "context.h"
#include "cohort.h"
#include "scenario.h"

//================================================================//
// ConsensusApp
//================================================================//
class ConsensusApp :
	public Poco::Util::Application {
public:

	//----------------------------------------------------------------//
	int main ( const vector < string > &args ) {
		FastGangScenario scenario;
		scenario.Run ();
		return EXIT_OK;
	}
};

POCO_APP_MAIN ( ConsensusApp );
