// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "chain.h"
#include "cohort.h"
#include "context.h"
#include "player.h"

//================================================================//
// Player
//================================================================//

//----------------------------------------------------------------//
int Player::GetID () const {
    
    return this->mID;
}

//----------------------------------------------------------------//
uint Player::GetScore ( int entropy ) {

    return Context::GetScore ( this->mID, entropy );
}

//----------------------------------------------------------------//
const Player* Player::GetNextPlayerInCycle () {

    this->mNewBatch = this->mPlayersCheckedCount == 0;

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
Player::Player () :
    mPlayersCheckedCount ( 0 ),
    mID ( -1 ),
    mCohort ( 0 ),
    mFrequency ( 1 ),
    mNewBatch ( false ),
    mVerbose ( false ),
    mStepStyle ( STEP_NORMAL ) {
}

//----------------------------------------------------------------//
void Player::Print () const {

    printf ( "[%s] ", this->mCohort ? this->mCohort->mName.c_str () : "" );
    this->mChain.Print ();
}

//----------------------------------------------------------------//
const Player* Player::RequestPlayer () {

    const Player* player = 0;
    
    do {
        player = this->GetNextPlayerInCycle ();
    } while ( !(( this->mCohort->mRequestFlags & player->mCohort->mGroupFlags ) && ( player->mCohort->mRespondFlags & this->mCohort->mGroupFlags )));
    
    return player;
}

//----------------------------------------------------------------//
void Player::SetStepStyle ( StepStyle stepStyle ) {

    this->mStepStyle = stepStyle;
}

//----------------------------------------------------------------//
void Player::SetVerbose ( bool verbose ) {

    this->mVerbose = verbose;
}

//----------------------------------------------------------------//
void Player::Step () {

    if ( !this->mCohort ) return;
    if ( this->mCohort->mIsPaused ) return;
    if ( Context::Drop ()) return;

    // start walking down both chains. if the cycles match, merge the
    // participants.
    // once we find a spot that doesn't match, we have to decide which chain
    // is better.
    // if one is locked and one is not, just pick the locked chain and go with that.
    // but if they are both locked... then most participants wins?
    // or do we need most participants by a margin?
    // if both the same, keep the status quo...
    // also: can we detect participants that should be in the chain but aren't? (seems like it)

    switch ( this->mStepStyle ) {
        case STEP_CAREFUL:
        case STEP_NORMAL:
            this->StepNormal ();
            break;
        case STEP_POLITE:
            this->StepPolite ();
            break;
    }
}

//----------------------------------------------------------------//
void Player::StepNormal () {

    // only push ourselves if we're not in a new cycle OR if we've
    // finished polling everyone.
    
    bool force = (( this->mStepStyle == STEP_NORMAL ) || this->mNewBatch );
    
    const Player* player = this->RequestPlayer ();
    if ( !player ) {
        this->mChain.Push ( this->mID, force );
        return;
    }
    
    Chain chain0 = this->mChain;
    Chain chain1 = player->mChain;

    if ( this->mVerbose ) {
        printf ( " player: %d\n", this->mID );
        chain0.Print ( "   CHAIN0: " );
        chain1.Print ( "   CHAIN1: " );
    }

    chain0.Push ( this->mID, force );
    chain1.Push ( this->mID, force );
    
    if ( this->mVerbose ) {
        chain0.Print ( "    NEXT0: " );
        chain1.Print ( "    NEXT1: " );
    }
    
    this->mChain = Chain::Choose ( chain0, chain1 );
    
    if ( this->mVerbose ) {
        this->mChain.Print ( "     BEST: " );
        printf ( "\n" );
    }
}

//----------------------------------------------------------------//
void Player::StepPolite () {

    // only change our chain once we've polled everyone. only push
    // ourselves once we've polled everyone.
    
    if ( this->mNewBatch ) {
        this->mNextChain.Push ( this->mID );
        this->mChain = this->mNextChain;
    }

    const Player* player = this->RequestPlayer ();
    if ( !player ) return;
    
    Chain chain0 = this->mNextChain;
    Chain chain1 = player->mChain;
    
    if ( this->mVerbose ) {
        printf ( " player: %d\n", this->mID );
        chain0.Print ( "   CHAIN0: " );
        chain1.Print ( "   CHAIN1: " );
    }
    
    chain0.Push ( this->mID, false );
    chain1.Push ( this->mID, false );
    
    if ( this->mVerbose ) {
        chain0.Print ( "    NEXT0: " );
        chain1.Print ( "    NEXT1: " );
    }
    
    this->mNextChain = Chain::Choose ( chain0, chain1 );
    
    if ( this->mVerbose ) {
        this->mChain.Print ( "     BEST: " );
        printf ( "\n" );
    }
}
