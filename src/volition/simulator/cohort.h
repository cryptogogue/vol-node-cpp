// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef COHORT_H
#define COHORT_H

#include <simulator/player.h>

//================================================================//
// Cohort
//================================================================//
class Cohort {
private:

    friend class Context;
    friend class Player;

    string  mName;

    int     mBasePlayer;
    bool    mIsPaused;
    
    int     mGroupFlags;
    int     mRequestFlags;
    int     mRespondFlags;

    list < Player* > mPlayers;

public:

    //----------------------------------------------------------------//
                    Cohort                  ();
    void            Pause                   ( bool paused );
    void            RandomizeFrequencies    ( int max );
    void            SetFlags                ( int group, int request, int respond );
    void            SetFrequency            ( int frequency );
    void            SetName                 ( string name );
    void            SetStepStyle            ( Player::StepStyle stepStyle );
    void            SetVerbose              ( bool verbose );
};

#endif
