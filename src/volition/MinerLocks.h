// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINERLOCKS_H
#define VOLITION_MINERLOCKS_H

#include <volition/common.h>
#include <volition/Miner.h>

namespace Volition {

//================================================================//
// ScopedExclusiveMinerLock
//================================================================//
class ScopedExclusiveMinerLock :
    public unique_lock < mutex > {
public:

    //----------------------------------------------------------------//
    ScopedExclusiveMinerLock ( shared_ptr < Miner > miner ) :
        unique_lock < std::mutex > ( miner->mMutex ) {
    }
};

//================================================================//
// ScopedSharedMinerBlockTreeLock
//================================================================//
class ScopedSharedMinerBlockTreeLock :
    public shared_lock < shared_mutex > {
public:

    //----------------------------------------------------------------//
    ScopedSharedMinerBlockTreeLock ( shared_ptr < Miner > miner ) :
        shared_lock < shared_mutex > ( miner->mBlockTreeMutex ) {
    }
};

//================================================================//
// ScopedSharedMinerLedgerLock
//================================================================//
class ScopedSharedMinerLedgerLock :
    public shared_lock < shared_mutex >,
    public LockedLedgerIterator {
public:

    //----------------------------------------------------------------//
    ScopedSharedMinerLedgerLock ( shared_ptr < Miner > miner ) :
        shared_lock < shared_mutex > ( miner->mLockedLedgerMutex ),
        LockedLedgerIterator ( miner->getLockedLedger ()) {
    }
};


//================================================================//
// ScopedSharedMinerStatusLock
//================================================================//
class ScopedSharedMinerStatusLock :
    public MinerStatus,
    public MinerSnapshot {
public:

    //----------------------------------------------------------------//
    ScopedSharedMinerStatusLock ( shared_ptr < Miner > miner ) {
        miner->getSnapshot ( this, this );
    }
};

} // namespace Volition
#endif
