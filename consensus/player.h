#ifndef PLAYER_H
#define PLAYER_H

#include "chain.h"
#include "cohort.h"

class Player;

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

	bool				mVerbose;

	//----------------------------------------------------------------//
	const Player*		GetNextPlayerInCycle	();
	const Player*		RequestPlayer			();
	void				Step					();

public:
		
	//----------------------------------------------------------------//
	int					GetID			() const;
	uint				GetScore		( int entropy );
	void				Init			( int playerID );
	void				Next			();
						Player			();
	void				Print			() const;
	void				SetVerbose		( bool verbose );
};

#endif
