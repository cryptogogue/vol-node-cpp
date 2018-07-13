//
//  main.cpp
// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>

namespace Volition {

//================================================================//
// State
//================================================================//

//----------------------------------------------------------------//
const map < string, MinerInfo >& State::getMinerInfo () const {

    return this->mMinerInfo;
}

//----------------------------------------------------------------//
const MinerInfo* State::getMinerInfo ( string minerID ) const {

    map < string, MinerInfo >::const_iterator minerInfoIt = this->mMinerInfo.find ( minerID );
    if ( minerInfoIt != this->mMinerInfo.end ()) {
        return &minerInfoIt->second;
    }
    return NULL;
}

//----------------------------------------------------------------//
const map < string, string >& State::getMinerURLs () const {

    return this->mMinerURLs;
}

//----------------------------------------------------------------//
void State::registerMiner ( const MinerInfo& minerInfo ) {

    this->mMinerInfo.insert ( pair < string, MinerInfo >( minerInfo.getMinerID (), minerInfo ));
    this->mMinerURLs [ minerInfo.getMinerID ()] = minerInfo.getURL ();
}

//----------------------------------------------------------------//
State::State () {
}

//----------------------------------------------------------------//
State::~State () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
