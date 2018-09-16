// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/ChainMetadata.h>

namespace Volition {

//================================================================//
// ChainMetadata
//================================================================//

//----------------------------------------------------------------//
void ChainMetadata::affirmCycle ( size_t cycleID ) {

    this->mCycleMetadata.resize ( cycleID + 1 );
}

//----------------------------------------------------------------//
void ChainMetadata::affirmParticipant ( size_t cycleID, string participant ) {

    this->affirmCycle ( cycleID );
    CycleMetadata& metadata = this->mCycleMetadata [ cycleID ];
    if ( metadata.mKnownParticipants.find ( participant ) == metadata.mKnownParticipants.end ()) {
        metadata.mKnownParticipants.insert ( participant );
    }
}

//----------------------------------------------------------------//
ChainMetadata::ChainMetadata () {
}

//----------------------------------------------------------------//
ChainMetadata::~ChainMetadata () {
}

} // namespace Volition
