#ifndef CONTEXT_H
#define CONTEXT_H

#include "common.h"

class Cohort;
class Player;
class TreeSummary;

//================================================================//
// Context
//================================================================//
class Context {
public:
    
    //----------------------------------------------------------------//
    static void             ApplyCohort             ( Cohort& cohort, string name, int basePlayer, int size = 1 );
    static int              CountPlayers            ();
    static bool             Drop                    ();
    static int              Entropy                 ( int height );
    static int              GetBlockID              ();
    static const Player&    GetPlayer               ( int playerID );
    static uint             GetScore                ( int playerID, int entropy );
    static float            GetThreshold            ();
    static void             InitPlayers             ( int nPlayers );
    static void             Print                   ();
    static void             PrintTree               ( bool verbose = false, int maxDepth = 0 );
    static void             Process                 ();
    static void             Reset                   ();
    static void             SetCyclesPerStep        ( int cycles );
    static void             SetDropRate             ( float percentage );
    static void             SetPlayerVerbose        ( int playerID, bool verbose );
    static void             SetScoreRandomizer      ( bool randomize );
    static void             SetThreshold            ( float threshold );
    static void             Summarize               ( TreeSummary& summary );
};

#endif
