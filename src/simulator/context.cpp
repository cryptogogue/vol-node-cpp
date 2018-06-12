// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <simulator/analysis.h>
#include <simulator/cohort.h>
#include <simulator/context.h>
#include <simulator/player.h>

static int                  sBlockCount         = 0;
static vector < Player >    sPlayers;
static map < int, int >     sEntropy;
float                       sDropRate           = 0.0;
int                         sCyclesPerStep      = 0;
bool                        sRandomizeScore     = false;
float                       sThreshold          = 0.75;

//================================================================//
// Context
//================================================================//

//----------------------------------------------------------------//
void Context::ApplyCohort ( Cohort& cohort, string name, int basePlayer, int size ) {

    cohort.mName        = name;
    cohort.mBasePlayer  = basePlayer;
    
    cohort.mPlayers.clear ();
    for ( int i = 0; i < size; ++i ) {
        int idx = i + basePlayer;
        sPlayers [ idx ].mCohort = &cohort;
        cohort.mPlayers.push_back ( &sPlayers [ idx ]);
    }
}

//----------------------------------------------------------------//
int Context::CountPlayers () {

    return ( int )sPlayers.size ();
}

//----------------------------------------------------------------//
bool Context::Drop () {

    return (( float )( rand () % ( 1000 + 1 )) / 1000.0 ) < sDropRate;
}

//----------------------------------------------------------------//
int Context::Entropy ( int height ) {

    if ( sEntropy.find ( height ) != sEntropy.end ()) {
        return sEntropy [ height ];
    }
    int entropy = rand ();
    sEntropy [ height ] = entropy;
    return entropy;
}

//----------------------------------------------------------------//
int Context::GetBlockID () {

    return sBlockCount++;
}

//----------------------------------------------------------------//
const Player& Context::GetPlayer ( int playerID ) {

    return sPlayers [ playerID ];
}

//----------------------------------------------------------------//
uint Context::GetScore ( int playerID, int entropy ) {

    if ( sRandomizeScore ) {
        return ( unsigned int )( playerID ^ entropy );
    }
    return ( uint )playerID;
}

//----------------------------------------------------------------//
float Context::GetThreshold () {

    return sThreshold;
}

//----------------------------------------------------------------//
void Context::InitPlayers ( int nPlayers ) {

    sPlayers.resize ( nPlayers );

    for ( int i = 0; i < nPlayers; ++i ) {
        sPlayers [ i ].Init ( i );
    }
}

//----------------------------------------------------------------//
void Context::Print () {

    int nPlayers = Context::CountPlayers ();
    for ( int i = 0; i < nPlayers; ++i ) {
        Player& player = sPlayers [ i ];
        player.Print ();
    }
}

//----------------------------------------------------------------//
void Context::PrintTree ( bool verbose, int maxDepth ) {

    TreeSummary summary;
    Context::Summarize ( summary );
    
    summary.PrintLevels ();
    summary.Print ( verbose, maxDepth );
}

//----------------------------------------------------------------//
void Context::Process () {

    int nPlayers = Context::CountPlayers ();
    int cycles = sCyclesPerStep ? sCyclesPerStep : nPlayers;
    
    for ( int i = 0; i < cycles; ++i ) {
    
        map < Player*, int > schedule;
        
        for ( int j = 0; j < nPlayers; ++j ) {
            Player& player = sPlayers [ j ];
            schedule [ &player ] = player.mFrequency;
        }
        
        while ( schedule.size ()) {
        
            map < Player*, int >::iterator scheduleIt = next ( schedule.begin (), rand () % schedule.size ());
            
            scheduleIt->first->Step ();
            scheduleIt->second -= 1;
            if ( scheduleIt->second == 0 ) {
                schedule.erase ( scheduleIt );
            }
        }
    }
}

//----------------------------------------------------------------//
void Context::Reset () {

    srand ( 1 );
    sPlayers.clear ();
    sEntropy.clear ();
    sBlockCount         = 0;
    sDropRate           = 0.0;
    sCyclesPerStep      = 1;
    sRandomizeScore     = false;
    sThreshold          = 0.75;
}

//----------------------------------------------------------------//
void Context::SetCyclesPerStep ( int cycles ) {

    sCyclesPerStep = cycles;
}

//----------------------------------------------------------------//
void Context::SetDropRate ( float percentage ) {

    sDropRate = percentage;
}

//----------------------------------------------------------------//
void Context::SetPlayerVerbose ( int playerID, bool verbose ) {

    sPlayers [ playerID ].mVerbose = verbose;
}

//----------------------------------------------------------------//
void Context::SetScoreRandomizer ( bool randomize ) {

    sRandomizeScore = randomize;
}

//----------------------------------------------------------------//
void Context::SetThreshold ( float threshold ) {

    sThreshold = 1.0 - threshold;
}

//----------------------------------------------------------------//
void Context::Summarize ( TreeSummary& summary ) {

    Tree tree;
    int nPlayers = Context::CountPlayers ();
    for ( int i = 0; i < nPlayers; ++i ) {
        Player& player = sPlayers [ i ];
        tree.AddChain ( player.mChain );
    }
    summary.Summarize ( tree );
}
