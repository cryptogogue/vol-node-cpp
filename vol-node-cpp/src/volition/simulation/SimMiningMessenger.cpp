// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Miner.h>
#include <volition/simulation/SimMiningMessenger.h>

namespace Volition {
namespace Simulation {

//================================================================//
// SimGetBlockTask
//================================================================//
class SimGetBlockTask {
private:

    friend class SimMiningMessenger;

    AbstractMiningMessengerClient&      mClient;
    string                              mMinerID;
    size_t                              mHeight;

public:

    //----------------------------------------------------------------//
    SimGetBlockTask ( AbstractMiningMessengerClient& client ) :
        mClient ( client ) {
    }
};


//================================================================//
// SimMiningMessenger
//================================================================//

//----------------------------------------------------------------//
void SimMiningMessenger::handleTask ( SimGetBlockTask& task ) {

    shared_ptr < Miner > miner = this->mMiners [ task.mMinerID ];
    assert ( miner );
    
    ScopedMinerLock scopedLock ( miner );
    const Chain& chain = *miner->getBestBranch ();

    shared_ptr < Block > block = chain.getBlock ( task.mHeight );
    assert ( block );
    
    task.mClient.receiveBlock ( task.mMinerID, block );
}

//----------------------------------------------------------------//
SimMiningMessenger::SimMiningMessenger () {
}

//----------------------------------------------------------------//
SimMiningMessenger::~SimMiningMessenger () {
}

//----------------------------------------------------------------//
void SimMiningMessenger::setMiners ( vector < shared_ptr < Miner >> miners ) {

    for ( size_t i = 0; i < miners.size (); ++i ) {
        shared_ptr < Miner > miner = miners [ i ];
        this->mMiners [ miner->getMinerID ()] = miner;
    }
}

//----------------------------------------------------------------//
void SimMiningMessenger::updateAndDispatch () {

    for ( list < shared_ptr < SimGetBlockTask >>::iterator cursor = this->mTasks.begin (); cursor != this->mTasks.end (); ) {
        list < shared_ptr < SimGetBlockTask >>::iterator taskIt = cursor++;
    
        shared_ptr < SimGetBlockTask > task = *taskIt;
        this->handleTask ( *task );
        this->mTasks.erase ( taskIt );
    }
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
void SimMiningMessenger::AbstractMiningMessenger_requestBlock ( AbstractMiningMessengerClient& client, string minerID, string url, size_t height ) {
    UNUSED ( url );

    shared_ptr < SimGetBlockTask > task = make_shared < SimGetBlockTask >( client );
    task->mMinerID  = minerID;
    task->mHeight   = height;
    
    this->mTasks.push_back ( task );
}

} // namespace Simulation
} // namespace Volition
