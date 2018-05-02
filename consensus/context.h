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
	static void				ApplyCohort				( Cohort& cohort, string name, int basePlayer, int topPlayer );
	static int				CountPlayers			();
	static bool				Drop					();
	static int				Entropy					( int height );
	static int				GetBlockID				();
	static const Player&	GetPlayer				( int playerID );
	static float			GetPlayerMerit			( int entropy, int playerID );
	static void				InitPlayers				( int nPlayers );
	static void				Print					();
	static void				PrintTree				( int maxDepth = 0 );
	static void				Process					();
	static void				Reset					();
	static void				SetCyclesPerStep		( int cycles );
	static void				SetDropRate				( float percentage );
};

#endif
