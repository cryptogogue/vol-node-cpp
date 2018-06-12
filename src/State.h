// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_STATE_H
#define VOLITION_STATE_H

#include "common.h"

namespace Volition {

//================================================================//
// State
//================================================================//
class State {
private:

    map < string, string >      mMinerInfo;

public:

    //----------------------------------------------------------------//
    map < string, string >      getMinerURLs            () const;
    void                        registerMiner           ( string minerID, string url );
                                State                   ();
                                //State                   ( const State* prevState );
                                ~State                  ();
};

} // namespace Volition
#endif
