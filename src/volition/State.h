// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_STATE_H
#define VOLITION_STATE_H

#include <volition/common.h>
#include <volition/MinerInfo.h>

namespace Volition {

//================================================================//
// State
//================================================================//
class State {
private:

    map < string, MinerInfo >   mMinerInfo;
    map < string, string >      mMinerURLs;

public:

    //----------------------------------------------------------------//
    const map < string, MinerInfo >&    getMinerInfo            () const;
    const MinerInfo*                    getMinerInfo            ( string minerID ) const;
    const map < string, string >&       getMinerURLs            () const;
    void                                registerMiner           ( const MinerInfo& minerInfo );
                                        State                   ();
                                        //State                   ( const State* prevState );
                                        ~State                  ();
};

} // namespace Volition
#endif
