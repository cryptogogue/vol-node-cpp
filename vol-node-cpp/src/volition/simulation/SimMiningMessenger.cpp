// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Miner.h>
#include <volition/simulation/SimMiningMessenger.h>

namespace Volition {
namespace Simulation {

//================================================================//
// SimMiningMessenger
//================================================================//

//----------------------------------------------------------------//
void SimMiningMessenger::handleTask ( const MiningMessengerRequest& request ) {

    shared_ptr < Miner > miner = this->mMiners [ request.mMinerID ];
    assert ( miner );
    
    ScopedMinerLock scopedLock ( miner );
    
    switch ( request.mRequestType ) {
        
        case MiningMessengerRequest::REQUEST_BLOCK: {
            
            const BlockTree& blockTree = miner->getBlockTree ();
            BlockTreeNode::ConstPtr node = blockTree.findNodeForHash ( request.mBlockDigest.toHex ());
            shared_ptr < const Block > block = node ? node->getBlock () : NULL;
            request.mClient->receive ( request, block, block );
            break;
        }
        
        case MiningMessengerRequest::REQUEST_HEADER: {
            
            BlockTreeNode::ConstPtr node = miner->getBestBranch ();
            request.mClient->receive ( request, node->getBlockHeader (), NULL );
            break;
        }
        
        default:
            assert ( false );
            break;
    }
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

    for ( list < shared_ptr < MiningMessengerRequest >>::iterator cursor = this->mTasks.begin (); cursor != this->mTasks.end (); ) {
        list < shared_ptr < MiningMessengerRequest >>::iterator taskIt = cursor++;
    
        shared_ptr < MiningMessengerRequest > task = *taskIt;
        this->handleTask ( *task );
        this->mTasks.erase ( taskIt );
    }
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
void SimMiningMessenger::AbstractMiningMessenger_request ( const MiningMessengerRequest& request ) {

    this->mTasks.push_back ( make_shared < MiningMessengerRequest >( request ));
}

} // namespace Simulation
} // namespace Volition
