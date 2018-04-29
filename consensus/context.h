#ifndef CONTEXT_H
#define CONTEXT_H

#include "common.h"

class Cohort;
class Player;

//================================================================//
// Context
//================================================================//
class Context {
public:
	
	//----------------------------------------------------------------//
	static void				ApplyCohort				( Cohort& cohort, int basePlayer, int topPlayer );
	static int				CountPlayers			();
	static bool				Drop					();
	static int				Entropy					( int height );
	static int				GetBlockID				();
	static const Player&	GetPlayer				( int playerID );
	static void				InitPlayers				( int nPlayers );
	static void				Print					();
	static void				PrintTree				( int maxDepth = 0 );
	static void				Process					( int cycles );
	static const Player*	RequestPlayer			( Player& requestedBy, int playerID );
	static void				SetDropRate				( float percentage );
};

#endif
