#ifndef COHORT_H
#define COHORT_H

#include "common.h"

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

#endif
