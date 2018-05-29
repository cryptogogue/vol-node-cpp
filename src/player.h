#ifndef PLAYER_H
#define PLAYER_H

#include "chain.h"

class Cohort;
class Player;

//================================================================//
// Player
//================================================================//
class Player {
public:

    enum StepStyle {
        STEP_CAREFUL,
        STEP_NORMAL,
        STEP_POLITE,
    };

private:

    friend class Cohort;
    friend class Context;

    int                 mID;
    int                 mPlayersCheckedCount;
    vector < bool >     mPlayersCheckedMask; // playerID -> checked
    Chain               mChain;
    Chain               mNextChain;

    Cohort*             mCohort;
    int                 mFrequency;

    bool                mNewBatch;
    bool                mVerbose;
    StepStyle           mStepStyle;

    //----------------------------------------------------------------//
    const Player*       GetNextPlayerInCycle    ();
    const Player*       RequestPlayer           ();
    void                Step                    ();
    void                StepNormal              ();
    void                StepPolite              ();

public:

    //----------------------------------------------------------------//
    int                 GetID           () const;
    uint                GetScore        ( int entropy );
    void                Init            ( int playerID );
                        Player          ();
    void                Print           () const;
    void                SetStepStyle    ( StepStyle stepStyle );
    void                SetVerbose      ( bool verbose );
};

#endif
