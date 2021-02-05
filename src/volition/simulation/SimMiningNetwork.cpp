// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Miner.h>
#include <volition/simulation/SimMiningMessenger.h>
#include <volition/UnsecureRandom.h>

namespace Volition {
namespace Simulation {

//================================================================//
// SimMiningNetwork
//================================================================//

//----------------------------------------------------------------//
void SimMiningNetwork::clearConstraint ( size_t base, size_t top ) {

    top = top < base ? base : top;
    for ( size_t i = base; i < top; ++i ) {
        this->mConstraintListsByIndex [ i ]->clear ();
    }
}

// TODO: restore constraint functionality

////----------------------------------------------------------------//
//void SimMiningNetwork::dispatchBlock ( const MiningMessengerRequest& request, shared_ptr < const Block > block ) {
//
//    const ConstraintList& constraintList = this->getMinerConstraints ( request );
//
//    for ( ConstraintList::const_iterator constraintIt = constraintList.cbegin (); constraintIt != constraintList.cend (); ++constraintIt ) {
//        const SimMiningNetworkConstraint& constraint = *constraintIt;
//
//        if (( constraint.mMode == SimMiningNetworkConstraint::CONSTRAINT_DROP_BLOCK ) && ( UnsecureRandom::get ().random () <= constraint.mProbability )) {
//            block = NULL;
//        }
//    }
//    request.mClient->receiveBlock ( request, block );
//}

////----------------------------------------------------------------//
//void SimMiningNetwork::dispatchHeaders ( const MiningMessengerRequest& request, list < shared_ptr < const BlockHeader >> headers ) {
//
//    const ConstraintList& constraintList = this->getMinerConstraints ( request );
//
//    for ( ConstraintList::const_iterator constraintIt = constraintList.cbegin (); constraintIt != constraintList.cend (); ++constraintIt ) {
//        const SimMiningNetworkConstraint& constraint = *constraintIt;
//
//        if (( constraint.mMode == SimMiningNetworkConstraint::CONSTRAINT_DROP_HEADER ) && (  UnsecureRandom::get ().random () <= constraint.mProbability )) {
//            headers.clear ();
//        }
//    }
//
//    list < shared_ptr < const BlockHeader >>::const_iterator headerIt = headers.cbegin ();
//    for ( ; headerIt != headers.cend (); ++headerIt ) {
//        request.mClient->receiveHeader ( request, *headerIt );
//    }
//}

//----------------------------------------------------------------//
void SimMiningNetwork::enqueueRequest ( AbstractMiningMessenger* messenger, const MiningMessengerRequest& request ) {

    this->mQueue.push_back ( pair < AbstractMiningMessenger*, MiningMessengerRequest >( messenger, request ));
}

//----------------------------------------------------------------//
shared_ptr < SimMiner > SimMiningNetwork::getMiner ( const MiningMessengerRequest& request ) {

    return this->mMinersByURL [ request.mMinerURL ];
}

//----------------------------------------------------------------//
const SimMiningNetwork::ConstraintList& SimMiningNetwork::getMinerConstraints ( const MiningMessengerRequest& request ) {

    shared_ptr < SimMiner > miner = this->getMiner ( request );
    assert ( miner );
    
    map < string, ConstraintList >::const_iterator constraintsIt = this->mConstraintLists.find ( miner->getMinerID ());
    assert ( constraintsIt != this->mConstraintLists.cend ());
    
    return constraintsIt->second;
}

//----------------------------------------------------------------//
void SimMiningNetwork::handleRequest ( AbstractMiningMessenger* client, const MiningMessengerRequest& request ) {

    shared_ptr < SimMiner > miner = this->getMiner ( request );
    assert ( miner );
    
    ScopedMinerLock scopedLock ( miner );
    string minerID = miner->getMinerID ();
    
    if ( !miner->mActive ) {
        client->enqueueErrorResponse ( request );
        return;
    }
    
    switch ( request.mRequestType ) {
        
        case MiningMessengerRequest::REQUEST_BLOCK: {
            
            string hash = request.mBlockDigest.toHex ();
            shared_ptr < const Block > block = NULL;
            
            const AbstractBlockTree& blockTree = miner->getBlockTree ();
            BlockTreeCursor cursor = blockTree.findCursorForHash ( hash );
            if ( cursor.hasHeader ()) {
                block = cursor.getBlock ();
            }
            client->enqueueBlockResponse ( request, block );
            break;
        }
        
        case MiningMessengerRequest::REQUEST_EXTEND_NETWORK: {
            
            set < string > miners = miner->sampleOnlineMinerURLs ( MINER_URL_BATCH_SIZE );
            client->enqueueExtendNetworkResponse ( request, miners );
            break;
        }
        
        case MiningMessengerRequest::REQUEST_HEADERS: {
            
            const Ledger& ledger = miner->getLedger ();
            size_t totalBlocks = ledger.countBlocks ();
            
            size_t base = request.mHeight;
            base = base < totalBlocks ? base : totalBlocks - 1;
            
            size_t top = base + HEADER_BATCH_SIZE;
            top = top <= totalBlocks ? top : totalBlocks;
            
            list < shared_ptr < const BlockHeader >> headers;
            for ( size_t i = base; i < top; ++i ) {
                shared_ptr < const BlockHeader > header = ledger.getHeader ( i );
                if ( !header ) break;
                headers.push_back ( header );
            }
            client->enqueueHeadersResponse ( request, headers );
            break;
        }
        
        case MiningMessengerRequest::REQUEST_MINER_INFO: {
        
            client->enqueueMinerInfoResponse ( request, minerID );
            break;
        }
        
        default:
            assert ( false );
            break;
    }
}

//----------------------------------------------------------------//
void SimMiningNetwork::pushConstraint ( const SimMiningNetworkConstraint& constraint, size_t base, size_t top ) {

    top = top < base ? base : top;
    for ( size_t i = base; i < top; ++i ) {
        this->mConstraintListsByIndex [ i ]->push_back ( constraint );
    }
}

//----------------------------------------------------------------//
void SimMiningNetwork::pushConstraint ( SimMiningNetworkConstraint::Mode mode, double probability, size_t base, size_t top ) {

    SimMiningNetworkConstraint constraint;
    constraint.mMode            = mode;
    constraint.mProbability     = probability;
        
    this->pushConstraint ( constraint, base, top );
}

//----------------------------------------------------------------//
void SimMiningNetwork::pushConstraintDropBlock ( double probability, size_t base, size_t top ) {

    this->pushConstraint ( SimMiningNetworkConstraint::CONSTRAINT_DROP_BLOCK, probability, base, top );
}

//----------------------------------------------------------------//
void SimMiningNetwork::pushConstraintDropHeader ( double probability, size_t base, size_t top ) {

    this->pushConstraint ( SimMiningNetworkConstraint::CONSTRAINT_DROP_BLOCK, probability, base, top );
}

//----------------------------------------------------------------//
void SimMiningNetwork::setMiner ( shared_ptr < SimMiner > miner ) {

    size_t i = this->mMinersByURL.size ();
    this->mConstraintListsByIndex.resize ( i + 1 );
    
    shared_ptr < SimMiner > simMiner = dynamic_pointer_cast < SimMiner >( miner );
    assert ( simMiner );
    
    this->mMinersByURL [ miner->getURL ()]  = simMiner;
    this->mConstraintListsByIndex [ i ]     = &this->mConstraintLists [ miner->getMinerID ()];
}

//----------------------------------------------------------------//
SimMiningNetwork::SimMiningNetwork () {
}

//----------------------------------------------------------------//
SimMiningNetwork::~SimMiningNetwork () {
}

//----------------------------------------------------------------//
void SimMiningNetwork::updateAndDispatch () {

    for ( ; this->mQueue.size (); this->mQueue.pop_front ()) {
        pair < AbstractMiningMessenger*, MiningMessengerRequest > entry = this->mQueue.front ();
        this->handleRequest ( entry.first, entry.second );
    }
}

} // namespace Simulation
} // namespace Volition
