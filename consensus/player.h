#ifndef PLAYER_H
#define PLAYER_H

#include "analysis.h"

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
	void			SetVerbose				( bool verbose );
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
	Analysis			mAnalysis;

	Cohort*				mCohort;
	int					mFrequency;

	bool				mVerbose;

	//----------------------------------------------------------------//
	const Player*	GetNextPlayerInCycle	();
	const Player*	RequestPlayer			();
	void			Step					();

public:
	
	//----------------------------------------------------------------//
	int				GetID			() const;
	void			Init			( int playerID );
	void			Next			();
					Player			();
	void			Print			() const;
	void			SetVerbose		( bool verbose );
};

#endif
