#ifndef CHAIN_H
#define CHAIN_H

#include "common.h"

class Player;

//================================================================//
// Block
//================================================================//
class Block {
public:

	int			mHeight;
	int			mEntropy;
	int			mPlayerID;
	int			mBlockID;
	float		mMerit;
	
	//----------------------------------------------------------------//
				Block				();
	float		GetNextMerit		( int playerID ) const;
};

//================================================================//
// Chain
//================================================================//
class Chain {

	//----------------------------------------------------------------//
	float		GetNextMerit		( int playerID, int& trimIndex ) const;

public:

	list < Block >		mBlocks;
	float				mMerit;

	//----------------------------------------------------------------//
	void		CopyFrom			( const Chain& from );
	float		GetNextMerit		( int playerID ) const;
				Chain				();
	void		Print				();
	void		PushBlock			( int playerID );
	void		Update				( const list < Block >& blocks );
};

#endif

