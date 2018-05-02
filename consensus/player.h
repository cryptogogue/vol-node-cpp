#ifndef PLAYER_H
#define PLAYER_H

#include "chain.h"

class Player;

//================================================================//
// Cohort
//================================================================//
class Cohort {
private:

	friend class Context;
	friend class Player;

	string	mName;

	int		mBasePlayer;
	int		mTopPlayer;

	bool	mIsPaused;
	
	int		mGroupFlags;
	int		mRequestFlags;
	int		mRespondFlags;

	list < Player* > mPlayers;

public:

	//----------------------------------------------------------------//
					Cohort					();
	void			Pause					( bool paused );
	void			RandomizeFrequencies	( int max );
	void			SetFlags				( int group, int request, int respond );
	void			SetFrequency			( int frequency );
	void			SetName					( string name );
};

//================================================================//
// Player
//================================================================//
class Player {
private:

	friend class Cohort;
	friend class Context;

	int					mID;
	int					mPlayersCheckedCount;
	vector < bool > 	mPlayersCheckedMask; // playerID -> checked
	Chain				mChain;

	Cohort*				mCohort;
	int					mFrequency;

	//----------------------------------------------------------------//
	const Player*	GetNextPlayerInCycle	();
	const Player*	RequestPlayer			();
	void			Step					();

public:
	
	//----------------------------------------------------------------//
	const Chain&	GetChain		() const;
	int				GetID			() const;
	void			Init			( int playerID );
	void			Next			();
					Player			();
	void			Print			() const;
};

#endif
