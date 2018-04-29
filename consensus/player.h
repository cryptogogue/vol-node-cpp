#ifndef PLAYER_H
#define PLAYER_H

#include "chain.h"

//================================================================//
// Cohort
//================================================================//
class Cohort {
private:

	friend class Context;
	friend class Player;

	int		mBasePlayer;
	int		mTopPlayer;
	
	bool	mPaused;

public:

	//----------------------------------------------------------------//
					Cohort		();
	void			Pause		( bool paused );
};

//================================================================//
// Player
//================================================================//
class Player {
private:

	friend class Context;

	int					mID;
	int					mPlayersCheckedCount;
	vector < bool > 	mPlayersCheckedMask; // playerID -> checked
	Chain				mChain;

	Cohort*				mCohort;

	//----------------------------------------------------------------//
	const Player*	GetNextPlayerInCycle	();

public:
	
	//----------------------------------------------------------------//
	const Chain&	GetChain	() const;
	int				GetID		() const;
	void			Init		( int playerID );
	void			Next		();
					Player		();
	void			Print		() const;
};

#endif
