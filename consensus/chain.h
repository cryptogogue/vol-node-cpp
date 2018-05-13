#ifndef CHAIN_H
#define CHAIN_H

#include "common.h"

//================================================================//
// Cycle
//================================================================//
class Cycle {
private:

	friend class Chain;

	vector < int >	mPlayerList;

	int				mCycleCount;
	int				mEntropy;

public:

	//----------------------------------------------------------------//
	static int		Compare				( const Cycle& cycle0, const Cycle& cycle1 );
	bool			Contains			( int playerID ) const;
					Cycle				();
					Cycle				( int cycleCount );
	int				FindPosition		( int playerID );
	void			Insert				( int playerID );
	void			Print				() const;
	int				Size				() const;
};

//================================================================//
// Chain
//================================================================//
class Chain {
private:

	vector < Cycle >		mCycles;

	//----------------------------------------------------------------//
	Cycle*					GetTop			();
	const Cycle*			GetTop			() const;

public:

	//----------------------------------------------------------------//
							Chain			();
	static const Chain&		Compare			( const Chain& chain0, const Chain& chain1 );
	int						FindPosition	( int playerID ) const;
	bool					InTopCycle		( int playerID ) const;
	void					Print			( const char* pre = 0, const char* post = "\n" ) const;
	void					Push			( int playerID );
};

#endif

