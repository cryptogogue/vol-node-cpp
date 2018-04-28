//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "context.h"
#include "player.h"

//================================================================//
// main
//================================================================//

//----------------------------------------------------------------//
int main ( int argc, const char* argv []) {

	Context::InitPlayers ( 16 );
	
	for ( int i = 0; true; ++i ) {
	
		Context::Process ();
		
		printf ( "ROUND: %d\n", i );
		//Context::PrintTree ();
		Context::Print ();
		printf ( "\n" );
	}

	return 0;
}
