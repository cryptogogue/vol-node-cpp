#ifndef CONTEXT_H
#define CONTEXT_H

#include "common.h"

class Player;

//================================================================//
// Context
//================================================================//
class Context {
public:
	
	//----------------------------------------------------------------//
	static int				CountPlayers	();
	static int				Entropy			( int height );
	static int				GetBlockID		();
	static const Player&	GetPlayer		( int playerID );
	static void				InitPlayers		( int nPlayers );
	static void				Print			();
	static void				PrintTree		( int maxDepth = 0 );
	static void				Process			();
};

#endif
