#ifndef PLAYER_H
#define PLAYER_H

#include "analysis.h"
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
	Chain				mChain;
	Analysis			mAnalysis;

	Cohort*				mCohort;
	int					mFrequency;

	bool				mVerbose;

	//----------------------------------------------------------------//
	void			CopyChainFrom			( const Chain* chain );
	const Player*	GetNextPlayerInCycle	();
	void			PushBlock				( const Chain* chain );
	const Player*	RequestPlayer			();
	void			Step					();
	void			StepCycles				( const Analysis* analysis0, const Analysis* analysis1 );
	bool			StepMerit				( const Chain* chain0, const Chain* chain1 );
	bool			StepMeritDelta			( const Chain* chain0, const Chain* chain1 );
	bool			StepParticipation		( const Chain* chain0, const Chain* chain1 );
	bool			StepScore				( const Chain* chain0, const Chain* chain1 );
	bool			StepSimple				( const Chain* chain0, const Chain* chain1 );

public:
	
	//----------------------------------------------------------------//
	const Chain&	GetChain		() const;
	int				GetID			() const;
	void			Init			( int playerID );
	void			Next			();
					Player			();
	void			Print			() const;
	void			SetVerbose		( bool verbose );
};

#endif
