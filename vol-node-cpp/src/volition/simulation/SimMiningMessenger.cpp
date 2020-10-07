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
void SimMiningMessenger::clearConstraint ( size_t base, size_t top ) {

    top = top < base ? base : top;
    for ( size_t i = base; i < top; ++i ) {
        this->mConstraintListsByIndex [ i ]->clear ();
    }
}

//----------------------------------------------------------------//
void SimMiningMessenger::dispatchBlock ( const MiningMessengerRequest& request, shared_ptr < const Block > block ) {

    ConstraintList& constraintList = this->mConstraintLists [ request.mMinerID ];

    for ( ConstraintList::iterator constraintIt = constraintList.begin (); constraintIt != constraintList.end (); ++constraintIt ) {
        SimMiningMessengerConstraint& constraint = *constraintIt;

        if (( constraint.mMode == SimMiningMessengerConstraint::CONSTRAINT_DROP_BLOCK ) && ( this->random () <= constraint.mProbability )) {
            block = NULL;
        }
    }
    request.mClient->receiveBlock ( request, block );
}

//----------------------------------------------------------------//
void SimMiningMessenger::dispatchHeaders ( const MiningMessengerRequest& request, list < shared_ptr < const BlockHeader >> headers ) {

    ConstraintList& constraintList = this->mConstraintLists [ request.mMinerID ];

    for ( ConstraintList::iterator constraintIt = constraintList.begin (); constraintIt != constraintList.end (); ++constraintIt ) {
        SimMiningMessengerConstraint& constraint = *constraintIt;

        if (( constraint.mMode == SimMiningMessengerConstraint::CONSTRAINT_DROP_HEADER ) && ( this->random () <= constraint.mProbability )) {
            headers.clear ();
        }
    }
    request.mClient->receiveHeaders ( request, headers );
}

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
            this->dispatchBlock ( request, block );
            break;
        }
        
        case MiningMessengerRequest::REQUEST_HEADERS: {
        
            BlockTreeNode::ConstPtr node = miner->getBestBranch ();
            
            list < shared_ptr < const BlockHeader >> headers;
            while ( node && ( headers.size () < HEADER_BATCH_SIZE )) {
                headers.push_front ( node->getBlockHeader ());
                node = node->getParent ();
            }
            this->dispatchHeaders ( request, headers );
            break;
        }
        
        case MiningMessengerRequest::REQUEST_PREV_HEADERS: {
            
            BlockTreeNode::ConstPtr node = miner->getBestBranch ();
            
            size_t top = request.mHeight;
            size_t base = HEADER_BATCH_SIZE < top ? top - HEADER_BATCH_SIZE : 0;
            
            list < shared_ptr < const BlockHeader >> headers;
            while ( node && ( base <= ( **node ).getHeight ())) {
                if (( **node ).getHeight () < top ) {
                    headers.push_front ( node->getBlockHeader ());
                }
                node = node->getParent ();
            }
            this->dispatchHeaders ( request, headers );
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
double SimMiningMessenger::random () {

    return this->mUniformDistribution ( this->mPRNG );
}

//----------------------------------------------------------------//
SimMiningMessenger::SimMiningMessenger () :
    mPRNG ( 0 ),
    mUniformDistribution ( 0, 1 ) {
    
//    seed = random_device ()()
}

//----------------------------------------------------------------//
SimMiningMessenger::~SimMiningMessenger () {
}

//----------------------------------------------------------------//
void SimMiningMessenger::setMiners ( vector < shared_ptr < Miner >> miners ) {

    this->mConstraintListsByIndex.resize ( miners.size ());

    for ( size_t i = 0; i < miners.size (); ++i ) {
    
        shared_ptr < Miner > miner = miners [ i ];
        string minerID = miner->getMinerID ();
        
        this->mMiners [ minerID ]               = miner;
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
