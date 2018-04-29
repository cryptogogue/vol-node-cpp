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
	Context::SetDropRate ( 0.8 );
	
	Cohort cohort;
	Context::ApplyCohort ( cohort, 0, 11 );
	
	for ( int i = 0; true; ++i ) {
	
		switch ( i ) {
			case 0:
				cohort.Pause ( true );
				break;
			
//			case 32:
//				cohort.Pause ( false );
//				break;
		}
	
		Context::Process ( 16 );
		
		printf ( "ROUND: %d\n", i );
		//Context::PrintTree ( 2 );
		Context::Print ();
		printf ( "\n" );
	}

	return 0;
}
