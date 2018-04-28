#ifndef PLAYER_H
#define PLAYER_H

#include "chain.h"

//================================================================//
// Player
//================================================================//
class Player {
private:

	int					mPlayersCheckedCount;
	vector < bool > 	mPlayersCheckedMask; // playerID -> checked

	//----------------------------------------------------------------//
	const Player&	GetNextPlayerInCycle	();

public:

	int		mID;
	
	map < int, Chain >	mState; // playerID -> chain
	
	//----------------------------------------------------------------//
	void			Init		( int playerID );
	void			Next		();
					Player		();
	void			Print		();
	const Chain&	State		() const;
};

#endif

