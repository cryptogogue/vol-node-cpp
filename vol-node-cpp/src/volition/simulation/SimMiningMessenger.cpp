// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Miner.h>
#include <volition/simulation/SimMiningMessenger.h>
#include <volition/UnsecureRandom.h>

namespace Volition {
namespace Simulation {

//================================================================//
// SimMiningMessenger
//================================================================//

//----------------------------------------------------------------//
void SimMiningMessenger::clearConstraint ( size_t base, size_t top ) {

    top = top < base ? base : top;
    for ( size_t i = base; i < top; ++i ) {
        this->mConstraintListsByIndex [ i ]->clear ();
    }
}

// TODO: restore constraint functionality

////----------------------------------------------------------------//
//void SimMiningMessenger::dispatchBlock ( const MiningMessengerRequest& request, shared_ptr < const Block > block ) {
//
//    const ConstraintList& constraintList = this->getMinerConstraints ( request );
//
//    for ( ConstraintList::const_iterator constraintIt = constraintList.cbegin (); constraintIt != constraintList.cend (); ++constraintIt ) {
//        const SimMiningMessengerConstraint& constraint = *constraintIt;
//
//        if (( constraint.mMode == SimMiningMessengerConstraint::CONSTRAINT_DROP_BLOCK ) && ( UnsecureRandom::get ().random () <= constraint.mProbability )) {
//            block = NULL;
//        }
//    }
//    request.mClient->receiveBlock ( request, block );
//}

////----------------------------------------------------------------//
//void SimMiningMessenger::dispatchHeaders ( const MiningMessengerRequest& request, list < shared_ptr < const BlockHeader >> headers ) {
//
//    const ConstraintList& constraintList = this->getMinerConstraints ( request );
//
//    for ( ConstraintList::const_iterator constraintIt = constraintList.cbegin (); constraintIt != constraintList.cend (); ++constraintIt ) {
//        const SimMiningMessengerConstraint& constraint = *constraintIt;
//
//        if (( constraint.mMode == SimMiningMessengerConstraint::CONSTRAINT_DROP_HEADER ) && (  UnsecureRandom::get ().random () <= constraint.mProbability )) {
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
shared_ptr < SimMiner > SimMiningMessenger::getMiner ( const MiningMessengerRequest& request ) {

    return this->mMinersByURL [ request.mMinerURL ];
}

//----------------------------------------------------------------//
const SimMiningMessenger::ConstraintList& SimMiningMessenger::getMinerConstraints ( const MiningMessengerRequest& request ) {

    shared_ptr < SimMiner > miner = this->getMiner ( request );
    assert ( miner );
    
    map < string, ConstraintList >::const_iterator constraintsIt = this->mConstraintLists.find ( miner->getMinerID ());
    assert ( constraintsIt != this->mConstraintLists.cend ());
    
    return constraintsIt->second;
}

//----------------------------------------------------------------//
void SimMiningMessenger::handleTask ( const MiningMessengerRequest& request ) {

    shared_ptr < SimMiner > miner = this->getMiner ( request );
    assert ( miner );
    
    ScopedMinerLock scopedLock ( miner );
    string minerID = miner->getMinerID ();
    
    if ( !miner->mActive ) {
        request.mClient->receiveError ( request );
        return;
    }
    
    switch ( request.mRequestType ) {
        
        case MiningMessengerRequest::REQUEST_BLOCK: {
            
            const BlockTree& blockTree = miner->getBlockTree ();
            BlockTreeNode::ConstPtr node = blockTree.findNodeForHash ( request.mBlockDigest.toHex ());
            shared_ptr < const Block > block = node ? node->getBlock () : NULL;
            request.mClient->receiveBlock ( request, block );
            break;
        }
        
        case MiningMessengerRequest::REQUEST_HEADERS: {
        
            BlockTreeNode::ConstPtr node = miner->getBestBranch ();
            
            list < shared_ptr < const BlockHeader >> headers;
            while ( node && ( headers.size () < HEADER_BATCH_SIZE )) {
                request.mClient->receiveHeader ( request, node->getBlockHeader ());
                node = node->getParent ();
            }
            break;
        }
        
        case MiningMessengerRequest::REQUEST_PREV_HEADERS: {
            
            BlockTreeNode::ConstPtr node = miner->getBestBranch ();
            
            size_t top = request.mHeight;
            size_t base = HEADER_BATCH_SIZE < top ? top - HEADER_BATCH_SIZE : 0;
            
            list < shared_ptr < const BlockHeader >> headers;
            while ( node && ( base <= ( **node ).getHeight ())) {
                if (( **node ).getHeight () < top ) {
                    request.mClient->receiveHeader ( request, node->getBlockHeader ());
                }
            }
            break;
        }
        
        case MiningMessengerRequest::REQUEST_MINER: {
        
            request.mClient->receiveMiner ( request, minerID, miner->getURL ());
            break;
        }
        
        case MiningMessengerRequest::REQUEST_MINER_URLS: {
        
            // no miners to discover in sim (for now)
            set < string > miners = miner->sampleActiveMinerURLs ( MINER_URL_BATCH_SIZE );
            set < string >::const_iterator urlIt = miners.cbegin ();
            for ( ; urlIt != miners.cend (); ++urlIt ) {
                request.mClient->receiveMinerURL ( request, *urlIt );
            }
            break;
        }
        
        default:
            assert ( false );
            break;
    }
}

//----------------------------------------------------------------//
void SimMiningMessenger::pushConstraint ( const SimMiningMessengerConstraint& constraint, size_t base, size_t top ) {

    top = top < base ? base : top;
    for ( size_t i = base; i < top; ++i ) {
        this->mConstraintListsByIndex [ i ]->push_back ( constraint );
    }
}

//----------------------------------------------------------------//
void SimMiningMessenger::pushConstraint ( SimMiningMessengerConstraint::Mode mode, double probability, size_t base, size_t top ) {

    SimMiningMessengerConstraint constraint;
    constraint.mMode            = mode;
    constraint.mProbability     = probability;
        
    this->pushConstraint ( constraint, base, top );
}

//----------------------------------------------------------------//
void SimMiningMessenger::pushConstraintDropBlock ( double probability, size_t base, size_t top ) {

    this->pushConstraint ( SimMiningMessengerConstraint::CONSTRAINT_DROP_BLOCK, probability, base, top );
}

//----------------------------------------------------------------//
void SimMiningMessenger::pushConstraintDropHeader ( double probability, size_t base, size_t top ) {

    this->pushConstraint ( SimMiningMessengerConstraint::CONSTRAINT_DROP_BLOCK, probability, base, top );
}

//----------------------------------------------------------------//
SimMiningMessenger::SimMiningMessenger () {
}

//----------------------------------------------------------------//
SimMiningMessenger::~SimMiningMessenger () {
}

//----------------------------------------------------------------//
void SimMiningMessenger::setMiners ( vector < shared_ptr < Miner >> miners ) {

    this->mConstraintListsByIndex.resize ( miners.size ());

    for ( size_t i = 0; i < miners.size (); ++i ) {
    
        shared_ptr < SimMiner > miner = dynamic_pointer_cast < SimMiner >( miners [ i ]);
        assert ( miner );
        
        string minerID  = miner->getMinerID ();
        string url      = miner->getURL ();
        
        this->mMinersByURL [ url ]              = miner;
        this->mConstraintListsByIndex [ i ]     = &this->mConstraintLists [ minerID ];
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
