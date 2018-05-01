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

	string	mName;

	int		mBasePlayer;
	int		mTopPlayer;

	bool	mIsPaused;
	
	int		mQueryFlags;
	int		mRespondFlags;
	int		mFrequency;

public:

	//----------------------------------------------------------------//
					Cohort			();
	void			Pause			( bool paused );
	void			SetFlags		( int query, int respond );
	void			SetFrequency	( int frequency );
	void			SetName			( string name );
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
	const Player*	RequestPlayer			();
	void			Step					();

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
