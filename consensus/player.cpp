//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "analysis.h"
#include "context.h"
#include "player.h"

//================================================================//
// Cohort
//================================================================//

//----------------------------------------------------------------//
Cohort::Cohort () :
	mBasePlayer ( -1 ),
	mTopPlayer ( -1 ),
	mIsPaused ( false ),
	mGroupFlags ( -1 ),
	mRequestFlags ( -1 ),
	mRespondFlags ( -1 ) {
}

//----------------------------------------------------------------//
void Cohort::Pause ( bool paused ) {

	this->mIsPaused = paused;
}

//----------------------------------------------------------------//
void Cohort::RandomizeFrequencies ( int max ) {

	list < Player* >::iterator playerIt = this->mPlayers.begin ();
	for ( ; playerIt != this->mPlayers.end (); ++playerIt ) {
		( *playerIt )->mFrequency = ( rand () % max ) + 1;
	}
}

//----------------------------------------------------------------//
void Cohort::SetFlags ( int group, int request, int respond ) {

	this->mGroupFlags		= group;
	this->mRequestFlags		= request;
	this->mRespondFlags		= respond;
}

//----------------------------------------------------------------//
void Cohort::SetFrequency ( int frequency ) {

	list < Player* >::iterator playerIt = this->mPlayers.begin ();
	for ( ; playerIt != this->mPlayers.end (); ++playerIt ) {
		( *playerIt )->mFrequency = frequency;
	}
}

//----------------------------------------------------------------//
void Cohort::SetName ( string name ) {

	this->mName = name;
}

//----------------------------------------------------------------//
void Cohort::SetVerbose ( bool verbose ) {

	list < Player* >::iterator playerIt = this->mPlayers.begin ();
	for ( ; playerIt != this->mPlayers.end (); ++playerIt ) {
		( *playerIt )->SetVerbose ( verbose );
	}
}

//================================================================//
// Player
//================================================================//

//----------------------------------------------------------------//
void Player::CopyChainFrom ( const Chain* chain ) {

	if ( chain != &this->mChain ) {
		this->mChain.CopyFrom ( *chain );
	}
}

//----------------------------------------------------------------//
const Chain& Player::GetChain () const {
	
	return this->mChain;
}

//----------------------------------------------------------------//
int Player::GetID () const {
	
	return this->mID;
}

//----------------------------------------------------------------//
const Player* Player::GetNextPlayerInCycle () {

	int nPlayers = Context::CountPlayers ();
	this->mPlayersCheckedMask.resize ( nPlayers, false );
	
	// mark self as checked
	if ( this->mPlayersCheckedCount == 0 ) {
		this->mPlayersCheckedMask [ this->mID ] = true;
		this->mPlayersCheckedCount = 1;
	}
	
	int playerID = 0;
	do {
		playerID = rand () % nPlayers;
	} while ( this->mPlayersCheckedMask [ playerID ]);
	
	this->mPlayersCheckedCount++;
	this->mPlayersCheckedMask [ playerID ] = true;
	
	if ( this->mPlayersCheckedCount >= nPlayers ) {
		this->mPlayersCheckedMask.clear ();
		this->mPlayersCheckedCount = 0;
	}
	
	return &Context::GetPlayer ( playerID );
}

//----------------------------------------------------------------//
void Player::Init ( int playerID ) {

	this->mID = playerID;
}

//----------------------------------------------------------------//
void Player::Next () {

	if ( !this->mCohort ) return;
	if ( this->mCohort->mIsPaused ) return;

	for ( int i = 0; i < this->mFrequency; ++i ) {
		this->Step ();
	}
}

//----------------------------------------------------------------//
Player::Player () :
	mPlayersCheckedCount ( 0 ),
	mID ( -1 ),
	mCohort ( 0 ),
	mFrequency ( 1 ),
	mVerbose ( false ) {
}

//----------------------------------------------------------------//
void Player::Print () const {

	printf ( "[%s] ", this->mCohort ? this->mCohort->mName.c_str () : "" );
	this->mAnalysis.Print ();
}

//----------------------------------------------------------------//
void Player::PushBlock ( const Chain* chain ) {

	this->CopyChainFrom ( chain );
	this->mChain.PushBlock ( this->mID );
}

//----------------------------------------------------------------//
const Player* Player::RequestPlayer () {

	const Player* player = 0;
	
	do {
		player = this->GetNextPlayerInCycle ();
	} while ( !(( this->mCohort->mRequestFlags & player->mCohort->mGroupFlags ) && ( player->mCohort->mRespondFlags & this->mCohort->mGroupFlags )));
	
	return player;
}

float lerp ( float a, float b, float t ) {

	return a + (( b - a ) * t );
}

//----------------------------------------------------------------//
void Player::SetVerbose ( bool verbose ) {

	this->mVerbose = verbose;
}

//----------------------------------------------------------------//
void Player::Step () {

	if ( Context::Drop ()) return;

	const Player* player = this->RequestPlayer ();
	if ( !player ) return;

	//const Chain* chain0			= &this->mChain;
	//const Chain* chain1			= &player->GetChain ();

//	if ( chain0->mBlocks.size () < chain1->mBlocks.size ()) {
//		this->StepParticipation ( chain0, chain1 );
//	}
//	else {
//		this->StepMerit ( chain0, chain1 );
//	}

	//if ( !this->StepParticipation ( chain0, chain1 )) {
		this->StepCycles ( &this->mAnalysis, &player->mAnalysis );
	//}
}

//----------------------------------------------------------------//
void Player::StepCycles ( const Analysis* analysis0, const Analysis* analysis1 ) {

	if ( this->mVerbose ) {
		printf ( " player: %d\n", this->mID );
		analysis0->Print		( "   CHAIN0: " );
		analysis1->Print		( "   CHAIN1: " );
	}

	Analysis nextAnalysis0 = *analysis0;
	Analysis nextAnalysis1 = *analysis1;
	
	bool inTop0 = nextAnalysis0.InTopCycle ( this->mID );
	bool inTop1 = nextAnalysis1.InTopCycle ( this->mID );
	
	if ( inTop0 == inTop1 ) {

		nextAnalysis0.Push ( this->mID );
		nextAnalysis1.Push ( this->mID );
	}
	else {

		if ( !inTop0 ) {
			nextAnalysis0.Push ( this->mID );
		}
		if ( !inTop1 ) {
			nextAnalysis1.Push ( this->mID );
		}
	}

	if ( this->mVerbose ) {
		nextAnalysis0.Print		( "    NEXT0: " );
		nextAnalysis1.Print		( "    NEXT1: " );
	}
	
	const Analysis& best = Analysis::Compare ( nextAnalysis0, nextAnalysis1 );
	
	if ( this->mVerbose ) {
		best.Print				( "     BEST: " );
		printf ( "\n" );
	}
	
	this->mAnalysis = best;
}

//----------------------------------------------------------------//
//void Player::Step () {
//
//	if ( Context::Drop ()) return;
//
//	const Player* player = this->RequestPlayer ();
//	if ( !player ) return;
//
//	const Chain* chain0			= &this->mChain;
//	const Chain* chain1			= &player->GetChain ();
//
//	float test					= 0.0;
//	bool push					= false;
//
//	// chain 0, as is
//	float bestPenalty = chain0->mPenalty;
//	const Chain* bestChain = chain0;
//
//	// chain 1, as is
//	test = chain1->mPenalty;
//	if ( bestPenalty > test ) {
//		bestPenalty = test;
//		bestChain = chain1;
//	}
//
//	// chain 0, with push
//	test = chain0->GetNextPenalty ( this->mID );
//	if ( bestPenalty > test ) {
//		bestPenalty = test;
//		bestChain = chain0;
//		push = true;
//	}
//
//	// chain 1, with push
//	test = chain1->GetNextPenalty ( this->mID );
//	if ( bestPenalty > test ) {
//		bestPenalty = test;
//		bestChain = chain1;
//		push = true;
//	}
//
//	if ( bestChain != &this->mChain ) {
//		this->mChain.CopyFrom ( *bestChain );
//	}
//
//	if ( push ) {
//		this->mChain.PushBlock ( this->mID );
//	}
//}

//----------------------------------------------------------------//
//void Player::Step () {
//
//	if ( Context::Drop ()) return;
//
//	const Player* player = this->RequestPlayer ();
//	if ( !player ) return;
//
//	const Chain* chain0			= &this->mChain;
//	const Chain* chain1			= &player->GetChain ();
//
//	float test					= 0.0;
//	bool push					= false;
//	bool tie					= true;
//
//	// chain 0, as is
//	float bestWidth = chain0->mWidth;
//	const Chain* bestChain = chain0;
//
//	// chain 1, as is
//	test = chain1->mWidth;
//	if ( test != bestWidth ) {
//		tie = false;
//		if ( bestWidth < test ) {
//			bestWidth = test;
//			bestChain = chain1;
//		}
//	}
//
//	// chain 0, with push
//	test = chain0->GetNextWidth ( this->mID );
//	if ( test != bestWidth ) {
//		tie = false;
//		if ( bestWidth < test ) {
//			bestWidth = test;
//			bestChain = chain0;
//			push = true;
//		}
//	}
//
//	// chain 1, with push
//	test = chain1->GetNextWidth ( this->mID );
//	if ( test != bestWidth ) {
//		tie = false;
//		if ( bestWidth < test ) {
//			bestWidth = test;
//			bestChain = chain1;
//			push = true;
//		}
//	}
//
//	if ( tie ) {
//
//		// chain 0, as is
//		float bestDiversity = chain0->mDiversity;
//		bestChain = chain0;
//
//		// chain 1, as is
//		test = chain1->mDiversity;
//		if ( test != bestDiversity ) {
//			tie = false;
//			if ( bestDiversity < test ) {
//				bestDiversity = test;
//				bestChain = chain1;
//			}
//		}
//
//		// chain 0, with push
//		test = chain0->GetNextDiversity ( this->mID );
//		if ( test != bestDiversity ) {
//			tie = false;
//			if ( bestDiversity < test ) {
//				bestDiversity = test;
//				bestChain = chain0;
//				push = true;
//			}
//		}
//
//		// chain 1, with push
//		test = chain1->GetNextDiversity ( this->mID );
//		if ( test != bestDiversity ) {
//			tie = false;
//			if ( bestDiversity < test ) {
//				bestDiversity = test;
//				bestChain = chain1;
//				push = true;
//			}
//		}
//	}
//
//	if ( tie ) {
//
//		// chain 0, as is
//		float bestMerit = chain0->mMerit;
//		bestChain = chain0;
//
//		// chain 1, as is
//		test = chain1->mMerit;
//		if ( bestMerit < test ) {
//			bestMerit = test;
//			bestChain = chain1;
//		}
//
//		// chain 0, with push
//		test = chain0->GetNextMerit ( this->mID );
//		if ( bestMerit < test ) {
//			bestMerit = test;
//			bestChain = chain0;
//			push = true;
//		}
//
//		// chain 1, with push
//		test = chain1->GetNextMerit ( this->mID );
//		if ( bestMerit < test ) {
//			bestMerit = test;
//			bestChain = chain1;
//			push = true;
//		}
//	}
//
//	if ( bestChain != &this->mChain ) {
//		this->mChain.CopyFrom ( *bestChain );
//	}
//
//	if ( push ) {
//		this->mChain.PushBlock ( this->mID );
//	}
//}

//----------------------------------------------------------------//
//void Player::Step () {
//
//	if ( Context::Drop ()) return;
//
//	const Player* player = this->RequestPlayer ();
//	if ( !player ) return;
//
//	float testMerit				= 0.0;
//	float bestMerit				= 0.0;
//	const Chain* bestChain		= 0;
//	bool push					= false;
//
//	Chain* chain0				= &this->mChain;
//	const Chain* chain1			= &player->GetChain ();
//
//	float lengthDiff = ( float )chain1->mBlocks.size () - ( float )chain0->mBlocks.size ();
//	lengthDiff = lengthDiff < 0 ? -lengthDiff : lengthDiff;
//	lengthDiff = lengthDiff / 100.0;
//	lengthDiff = lengthDiff * lengthDiff;
//
//	float diversity0			= lengthDiff > 0.0 ? ( chain0->AverageSpan () * lengthDiff ) : 0.0;
//	float diversity1			= lengthDiff > 0.0 ? ( chain1->AverageSpan () * lengthDiff ) : 0.0;
//
////	bestMerit = chain0->GetNextMerit ( this->mID ) + diversity0;
////	bestChain = chain0;
////
////	testMerit = chain1->GetNextMerit ( this->mID ) + diversity1;
////	if ( bestMerit < testMerit ) {
////		bestMerit = testMerit;
////		bestChain = chain1;
////	}
////
////	if ( bestChain != &this->mChain ) {
////		this->mChain.CopyFrom ( *bestChain );
////	}
////	this->mChain.PushBlock ( this->mID );
//
//	// chain 0, as is
//	bestMerit = chain0->mMerit + diversity0;
//	bestChain = chain0;
//
//	// chain 1, as is
//	testMerit = chain1->mMerit + diversity1;
//	if ( bestMerit < testMerit ) {
//		bestMerit = testMerit;
//		bestChain = chain1;
//	}
//
//	// chain 0, with push
//	testMerit = chain0->GetNextMerit ( this->mID ) + diversity0;
//	if ( bestMerit < testMerit ) {
//		bestMerit = testMerit;
//		bestChain = chain0;
//		push = true;
//	}
//
//	// chain 1, with push
//	testMerit = chain1->GetNextMerit ( this->mID ) + diversity1;
//	if ( bestMerit < testMerit ) {
//		bestMerit = testMerit;
//		bestChain = chain1;
//		push = true;
//	}
//
//	if ( bestChain != &this->mChain ) {
//		this->mChain.CopyFrom ( *bestChain );
//	}
//
//	if ( push ) {
//		this->mChain.PushBlock ( this->mID );
//	}
//}

//----------------------------------------------------------------//
bool Player::StepMerit ( const Chain* chain0, const Chain* chain1 ) {

//	bool tie = true;
//
//	// chain 0, as is
//	float best = chain0->GetNextMerit ( this->mID );
//	const Chain* bestChain = chain0;
//
//	// chain 1, with push
//	float test = chain1->GetNextMerit ( this->mID );
//	if ( best != test ) {
//		tie = false;
//		if ( best > test ) {
//			best = test;
//			bestChain = chain1;
//		}
//	}
//
//	if ( tie ) {
//		bestChain = chain0->mBlocks.size () >= chain1->mBlocks.size () ? chain0 : chain1;
//	}
//
//	if ( bestChain != &this->mChain ) {
//		this->mChain.CopyFrom ( *bestChain );
//	}
//	this->mChain.PushBlock ( this->mID );
//	return true;


	float test		= 0.0;
	bool push		= false;

	// chain 0, as is
	float best = chain0->mMerit;
	const Chain* bestChain = chain0;

	// chain 1, as is
	test = chain1->mMerit;
	if ( best < test ) {
		best = test;
		bestChain = chain1;
	}

	// chain 0, with push
	test = chain0->GetNextMerit ( this->mID );
	if ( best < test ) {
		best = test;
		bestChain = chain0;
		push = true;
	}

	// chain 1, with push
	test = chain1->GetNextMerit ( this->mID );
	if ( best < test ) {
		best = test;
		bestChain = chain1;
		push = true;
	}

	if ( bestChain != &this->mChain ) {
		this->mChain.CopyFrom ( *bestChain );
	}

	if ( push ) {
		this->mChain.PushBlock ( this->mID );
		return true;
	}
	return false;
}

//----------------------------------------------------------------//
bool Player::StepMeritDelta ( const Chain* chain0, const Chain* chain1 ) {

	// chain 0, with push
	float best = ( chain0->GetNextMerit ( this->mID ) - chain0->mMerit ) * ( 1.0 + chain0->mParticipation );
	const Chain* bestChain = chain0;

	// chain 1, with push
	float test = ( chain1->GetNextMerit ( this->mID ) - chain1->mMerit )  * ( 1.0 + chain1->mParticipation );
	if ( best < test ) {
		best = test;
		bestChain = chain1;
	}

	if ( bestChain != &this->mChain ) {
		this->mChain.CopyFrom ( *bestChain );
	}

	this->mChain.PushBlock ( this->mID );
	return true;
}

//----------------------------------------------------------------//
bool Player::StepParticipation ( const Chain* chain0, const Chain* chain1 ) {

	bool tie = true;

	// chain 0, as is
	float best = chain0->GetNextParticipation ( this->mID ) - chain0->mParticipation;
	const Chain* bestChain = chain0;

	// chain 1, with push
	float test = chain1->GetNextParticipation ( this->mID ) - chain1->mParticipation;
	if ( best != test ) {
		tie = false;
		if ( best < test ) {
			best = test;
			bestChain = chain1;
		}
	}

//	float test		= 0.0;
//	bool push		= false;
//
//	// chain 0, as is
//	float best = chain0->mParticipation;
//	const Chain* bestChain = chain0;
//
//	// chain 1, as is
//	test = chain1->mParticipation;
//	if ( best < test ) {
//		best = test;
//		bestChain = chain1;
//	}
//
//	// chain 0, with push
//	test = chain0->GetNextParticipation ( this->mID );
//	if ( best < test ) {
//		best = test;
//		bestChain = chain0;
//		push = true;
//	}
//
//	// chain 1, with push
//	test = chain1->GetNextParticipation ( this->mID );
//	if ( best < test ) {
//		best = test;
//		bestChain = chain1;
//		push = true;
//	}

	if (( best > 0.0 ) && ( !tie )) {
		if ( bestChain != &this->mChain ) {
			this->mChain.CopyFrom ( *bestChain );
		}
		this->mChain.PushBlock ( this->mID );
	}
	return false;
}

////----------------------------------------------------------------//
//bool Player::StepScore ( const Chain* chain0, const Chain* chain1 ) {
//
//	float length0 = ( float )chain0->mBlocks.size ();
//	float length1 = ( float )chain1->mBlocks.size ();
//
//	if ( length0 != length1 ) {
//
//		const Chain* shortChain = chain0;
//		const Chain* longChain = chain1;
//
//		if ( length1 < length0 ) {
//			shortChain = chain1;
//			longChain = chain0;
//		}
//
//		if (( !shortChain->IsTop ( this->mID )) && ( shortChain->GetNextMerit ( this->mID ) > longChain->mMerit )) {
//			this->PushBlock ( shortChain );
//			return true;
//		}
//
//		if ( shortChain->mMerit > longChain->mMerit ) {
//			this->CopyChainFrom ( shortChain );
//			return true;
//		}
//
////		float shortMeritPerBlock = shortChain->mBlocks.size () ? shortChain->mMerit / ( float )shortChain->mBlocks.size () : 0.0;
////		float longMeritPerBlock = longChain->mBlocks.size () ? longChain->mMerit / ( float )longChain->mBlocks.size () : 0.0;
////
////		if ( shortMeritPerBlock < longMeritPerBlock ) {
////			this->mChain.CopyFrom ( *longChain );
////			return true;
////		}
//	}
//
//	if ( length == length )
//
//	// chain 0, with push
//	float meritPerBlock0 = chain0->GetNextMerit ( this->mID ) / ( length0 + 1.0 );
//
//	float meritPerBlock1 = chain1->GetNextMerit ( this->mID ) / ( length1 + 1.0 );
//
//	//const Chain* bestChain = ( part0 * merit0 ) >= ( part1 * merit1 ) ? chain0 : chain1;
//	const Chain* bestChain = meritPerBlock0 > meritPerBlock1 ? chain0 : chain1;
//
//	this->PushBlock ( bestChain );
//
//	return true;
//}

//----------------------------------------------------------------//
bool Player::StepScore ( const Chain* chain0, const Chain* chain1 ) {

	float length0 = ( float )chain0->mBlocks.size ();
	float length1 = ( float )chain1->mBlocks.size ();

	if ( this->mVerbose ) {
		printf ( " player: %d\n", this->mID );
		chain0->Print ( "   chain0: " );
		chain1->Print ( "   chain1: " );
	}

//	if ( length1 < length0 ) {
//
//		if ( this->mVerbose ) {
//			printf ( "   chain1 is shorter\n" );
//		}
//
//		if ( !chain1->IsTop ( this->mID )) {
//
//			float test = chain1->GetNextMerit ( this->mID );
//			if ( this->mVerbose ) {
//				chain1->Print ( "   chainA: ", 0 );
//				printf ( " + %d MERIT: %g\n", this->mID, test );
//			}
//
//			if ( test > chain0->mMerit ) {
//				if ( this->mVerbose ) {
//					printf ( "   ACCEPT chainA\n" );
//				}
//				this->PushBlock ( chain1 );
//				return true;
//			}
//		}
//
//		if ( chain1->mMerit > chain0->mMerit ) {
//			if ( this->mVerbose ) {
//				printf ( "   ACCEPT chain1\n" );
//			}
//			this->CopyChainFrom ( chain1 );
//			return true;
//		}
//
//		if ( this->mVerbose ) {
//			printf ( "   CHAIN 1 MERIT REJECTED\n" );
//		}
//	}
	
//	float meritPerBlock0 = chain0->IsTop ( this->mID ) ? chain0->mMerit / length0 : chain0->GetNextMerit ( this->mID ) / ( length0 + 1.0 );
//	float meritPerBlock1 = chain1->IsTop ( this->mID ) ? chain1->mMerit / length1 : chain1->GetNextMerit ( this->mID ) / ( length1 + 1.0 );
//
//	if ( this->mVerbose ) {
//		printf ( "   NO EARLY OUT\n" );
//
//		chain0->Print ( "   chain0: ", 0 );
//		if ( !chain0->IsTop ( this->mID )) {
//			printf ( " + %d MERIT PER BLOCK: %g\n", this->mID, meritPerBlock0 );
//		}
//		else {
//			printf ( " MERIT PER BLOCK: %g\n", meritPerBlock0 );
//		}
//
//		chain1->Print ( "   chain1: ", 0 );
//		if ( !chain1->IsTop ( this->mID )) {
//			printf ( " + %d MERIT PER BLOCK: %g\n", this->mID, meritPerBlock1 );
//		}
//		else {
//			printf ( " MERIT PER BLOCK: %g\n", meritPerBlock1 );
//		}
//	}
//
//	//const Chain* bestChain = ( part0 * merit0 ) >= ( part1 * merit1 ) ? chain0 : chain1;
//	const Chain* bestChain = meritPerBlock0 > meritPerBlock1 ? chain0 : chain1;
//
//	if ( !bestChain->IsTop ( this->mID )) {
//		this->PushBlock ( bestChain );
//	}
//	else {
//		this->CopyChainFrom ( bestChain );
//	}
//
//	if ( this->mVerbose ) {
//		this->mChain.Print ( "     BEST: " );
//	}
//	return true;

	float meritPerBlock0 = chain0->GetNextMerit ( this->mID ) / ( length0 + 1.0 );
	float meritPerBlock1 = chain1->GetNextMerit ( this->mID ) / ( length1 + 1.0 );

	if ( this->mVerbose ) {
		printf ( "   NO EARLY OUT\n" );
		
		chain0->Print ( "   chain0: ", 0 );
		printf ( " MERIT PER BLOCK: %g\n", meritPerBlock0 );

		chain1->Print ( "   chain1: ", 0 );
		printf ( " MERIT PER BLOCK: %g\n", meritPerBlock1 );
	}
	
	//const Chain* bestChain = ( part0 * merit0 ) >= ( part1 * merit1 ) ? chain0 : chain1;
	const Chain* bestChain = meritPerBlock0 > meritPerBlock1 ? chain0 : chain1;

	if ( !bestChain->IsTop ( this->mID )) {
		this->PushBlock ( bestChain );
	}

	if ( this->mVerbose ) {
		this->mChain.Print ( "     BEST: " );
	}
	return true;
}

//----------------------------------------------------------------//
bool Player::StepSimple ( const Chain* chain0, const Chain* chain1 ) {

	float length0 = ( float )chain0->mBlocks.size ();
	float length1 = ( float )chain1->mBlocks.size ();

	if ( this->mVerbose ) {
		printf ( " player: %d\n", this->mID );
		chain0->Print ( "   chain0: " );
		chain1->Print ( "   chain1: " );
	}

//	if ( length1 < length0 ) {
//
//		if ( this->mVerbose ) {
//			printf ( "   chain1 is shorter\n" );
//		}
//
//		if ( !chain1->IsTop ( this->mID )) {
//
//			float test = chain1->GetNextMerit ( this->mID );
//			if ( this->mVerbose ) {
//				chain1->Print ( "   chainA: ", 0 );
//				printf ( " + %d MERIT: %g\n", this->mID, test );
//			}
//
//			if ( test > chain0->mMerit ) {
//				if ( this->mVerbose ) {
//					printf ( "   ACCEPT chainA\n" );
//				}
//				this->PushBlock ( chain1 );
//				return true;
//			}
//		}
//
//		if ( chain1->mMerit > chain0->mMerit ) {
//			if ( this->mVerbose ) {
//				printf ( "   ACCEPT chain1\n" );
//			}
//			this->CopyChainFrom ( chain1 );
//			return true;
//		}
//
//		if ( this->mVerbose ) {
//			printf ( "   CHAIN 1 MERIT REJECTED\n" );
//		}
//	}
	
//	float meritPerBlock0 = chain0->IsTop ( this->mID ) ? chain0->mMerit / length0 : chain0->GetNextMerit ( this->mID ) / ( length0 + 1.0 );
//	float meritPerBlock1 = chain1->IsTop ( this->mID ) ? chain1->mMerit / length1 : chain1->GetNextMerit ( this->mID ) / ( length1 + 1.0 );
//
//	if ( this->mVerbose ) {
//		printf ( "   NO EARLY OUT\n" );
//
//		chain0->Print ( "   chain0: ", 0 );
//		if ( !chain0->IsTop ( this->mID )) {
//			printf ( " + %d MERIT PER BLOCK: %g\n", this->mID, meritPerBlock0 );
//		}
//		else {
//			printf ( " MERIT PER BLOCK: %g\n", meritPerBlock0 );
//		}
//
//		chain1->Print ( "   chain1: ", 0 );
//		if ( !chain1->IsTop ( this->mID )) {
//			printf ( " + %d MERIT PER BLOCK: %g\n", this->mID, meritPerBlock1 );
//		}
//		else {
//			printf ( " MERIT PER BLOCK: %g\n", meritPerBlock1 );
//		}
//	}
//
//	//const Chain* bestChain = ( part0 * merit0 ) >= ( part1 * merit1 ) ? chain0 : chain1;
//	const Chain* bestChain = meritPerBlock0 > meritPerBlock1 ? chain0 : chain1;
//
//	if ( !bestChain->IsTop ( this->mID )) {
//		this->PushBlock ( bestChain );
//	}
//	else {
//		this->CopyChainFrom ( bestChain );
//	}
//
//	if ( this->mVerbose ) {
//		this->mChain.Print ( "     BEST: " );
//	}
//	return true;

	float meritPerBlock0 = chain0->GetNextMerit ( this->mID ) / ( length0 + 1.0 );
	float meritPerBlock1 = chain1->GetNextMerit ( this->mID ) / ( length1 + 1.0 );

	if ( this->mVerbose ) {
		printf ( "   NO EARLY OUT\n" );
		
		chain0->Print ( "   chain0: ", 0 );
		printf ( " MERIT PER BLOCK: %g\n", meritPerBlock0 );

		chain1->Print ( "   chain1: ", 0 );
		printf ( " MERIT PER BLOCK: %g\n", meritPerBlock1 );
	}
	
	//const Chain* bestChain = ( part0 * merit0 ) >= ( part1 * merit1 ) ? chain0 : chain1;
	const Chain* bestChain = meritPerBlock0 > meritPerBlock1 ? chain0 : chain1;

	if ( !bestChain->IsTop ( this->mID )) {
		this->PushBlock ( bestChain );
	}

	if ( this->mVerbose ) {
		this->mChain.Print ( "     BEST: " );
	}
	return true;
}
