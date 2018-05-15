#ifndef CHAIN_H
#define CHAIN_H

#include "common.h"

//================================================================//
// Cycle
//================================================================//
class Cycle {
private:

	friend class Chain;
	friend class Tree;

	vector < int >	mPlayerList;

	int				mCycleID;
	int				mEntropy;

	float			mNewPlayerRatio;

public:

	//----------------------------------------------------------------//
	static int		Compare				( const Cycle& cycle0, const Cycle& cycle1 );
	bool			Contains			( int playerID ) const;
					Cycle				();
					Cycle				( int cycleID );
	int				FindPosition		( int playerID );
	void			Insert				( int playerID );
	void			Print				() const;
	int				Size				() const;
	void			UpdatePlayerRatio	( size_t prevCount );
};

//================================================================//
// Chain
//================================================================//
class Chain {
private:

	friend class Tree;

	vector < Cycle >		mCycles;

	//----------------------------------------------------------------//
	Cycle*					GetTopCycle		();
	const Cycle*			GetTopCycle		() const;
	void					Insert			( int cycleID, int playerID );

public:

	//----------------------------------------------------------------//
							Chain			();
	static const Chain&		Compare			( const Chain& chain0, const Chain& chain1 );
	int						FindPosition	( int playerID ) const;
	//bool					InTopCycle		( int playerID ) const;
	void					Print			( const char* pre = 0, const char* post = "\n" ) const;
	void					Push			( int playerID );
};

#endif

