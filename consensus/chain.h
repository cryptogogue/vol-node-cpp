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

	friend class Analysis;

	int					mSpanSum;
	int					mSpanCount;

	map < int, int >	mPlayerIdToBlockHeight;
	map < int, int >	mHistogram;

	//----------------------------------------------------------------//
	float		GetNextDiversity	( int playerID, int& spanSum, int& spanCount ) const;

public:

	vector < Block >	mBlocks;
	float				mDiversity;
	float				mMerit;
	float				mWidth;
	float				mPenalty;
	float				mScore;
	float				mParticipation;
	
	//----------------------------------------------------------------//
	float		AverageSpan					() const;
				Chain						();
	void		CopyFrom					( const Chain& from );
	float		FindBestScore				( int playerID, int& position ) const;
	float		GetNextDiversity			( int playerID ) const;
	float		GetNextMerit				( int playerID ) const;
	float		GetNextParticipation		( int playerID ) const;
	float		GetNextPenalty				( int playerID ) const;
	int			GetNextWidth				( int playerID ) const;
	void		InsertBlock					( int playerID, int position, float score );
	bool		IsTop						( int playerID ) const;
	void		Print						( const char* pre = 0, const char* post = "\n" ) const;
	void		PushBlock					( int playerID );
	void		Update						( const list < Block >& blocks );
};

#endif

