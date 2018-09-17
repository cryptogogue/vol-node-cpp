// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CHAINMETADATA_H
#define VOLITION_CHAINMETADATA_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// CycleMetadata
//================================================================//
class CycleMetadata {
private:

    friend class Chain;
    friend class ChainMetadata;

    set < string > mKnownParticipants;

public:

};

//================================================================//
// ChainMetadata
//================================================================//
class ChainMetadata {
private:

    friend class Chain;

    vector < CycleMetadata > mCycleMetadata;

public:

    //----------------------------------------------------------------//
    void    affirmCycle         ( size_t cycleID );
    void    affirmParticipant   ( size_t cycleID, string participant );
            ChainMetadata       ();
            ~ChainMetadata      ();
    bool    isParticipant       ( size_t cycleID, string minerID ) const;
};

} // namespace Volition
#endif
