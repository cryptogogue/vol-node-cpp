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
void SimMiningMessenger::dispatch ( const MiningMessengerRequest& request, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block ) {

    ConstraintList& constraintList = this->mConstraintLists [ request.mMinerID ];

    for ( ConstraintList::iterator constraintIt = constraintList.begin (); constraintIt != constraintList.end (); ++constraintIt ) {
        SimMiningMessengerConstraint& constraint = *constraintIt;

        switch ( constraint.mMode ) {
        
            case SimMiningMessengerConstraint::CONSTRAINT_DELAY:
                break;
            
            case SimMiningMessengerConstraint::CONSTRAINT_DROP_BLOCK:
                
                if ( this->random () <= constraint.mProbability ) {
                    block = NULL;
                }
                break;
            
            case SimMiningMessengerConstraint::CONSTRAINT_DROP_HEADER:
                
                if ( this->random () <= constraint.mProbability ) {
                    header = NULL;
                }
                break;
            
            case SimMiningMessengerConstraint::CONSTRAINT_DELAY_AND_DROP:
                break;
            
            case SimMiningMessengerConstraint::CONSTRAINT_NONE:
            default:
                break;
        }
    }
    request.mClient->receive ( request, header, block );
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
            this->dispatch ( request, block, block );
            break;
        }
        
        case MiningMessengerRequest::REQUEST_HEADER: {
            
            BlockTreeNode::ConstPtr node = miner->getBestBranch ();
            this->dispatch ( request, node->getBlockHeader (), NULL );
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
