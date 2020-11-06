// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/simulation/SimMiner.h>
#include <volition/UnsecureRandom.h>

namespace Volition {
namespace Simulation {

//================================================================//
// SimMiner
//================================================================//

//----------------------------------------------------------------//
void SimMiner::extendChain ( string charmHex, time_t time ) {

    shared_ptr < const Block > prevBlock = this->mChain->getBlock ();

    shared_ptr < Block > block = make_shared < Block >(
        this->mMinerID,
        this->mVisage,
        time,
        prevBlock ? prevBlock.get () : NULL,
        this->mKeyPair
    );
    
    Digest charm = block->getCharm ();
    std::fill ( charm.begin (), charm.end (), 0 );
    string compose = charm.toHex ();
    compose.replace ( 0, charmHex.size (), charmHex );
    charm.fromHex ( compose );
    
    block->setCharm ( charm );
    block->sign ( this->mKeyPair, Digest::DEFAULT_HASH_ALGORITHM );
    
    this->pushBlock ( block );
}

//----------------------------------------------------------------//
shared_ptr < Block > SimMiner::replaceBlock ( shared_ptr < const Block > oldBlock, string charmHex ) {
    
    assert ( oldBlock );
    
    BlockTreeNode::ConstPtr prevNode = (( const BlockTree& )this->mBlockTree ).findNodeForHash ( oldBlock->getPrevDigest ());
    assert ( prevNode );
    
    shared_ptr < const Block > prevBlock = prevNode->getBlock ();
    assert ( prevBlock );
    
    shared_ptr < Block > block = make_shared < Block >(
        this->mMinerID,
        this->mVisage,
        oldBlock->getTime (),
        prevBlock.get (),
        this->mKeyPair
    );
    
    Digest charm = block->getCharm ();
    std::fill ( charm.begin (), charm.end (), 0 );
    string compose = charm.toHex ();
    compose.replace ( 0, charmHex.size (), charmHex );
    charm.fromHex ( compose );
    
    block->setCharm ( charm );
    block->sign ( this->mKeyPair, Digest::DEFAULT_HASH_ALGORITHM );
    return block;
}

//----------------------------------------------------------------//
void SimMiner::rewindChain ( size_t height ) {

    while (( **this->mBestBranch ).getHeight () > height ) {
        this->mBestBranch = this->mBestBranch->getParent ();
    }
    this->composeChain ();
}

//----------------------------------------------------------------//
void SimMiner::setActive ( bool active ) {

    this->mActive = active;
}

//----------------------------------------------------------------//
void SimMiner::setCharm ( size_t height, string charmHex ) {

    BlockTreeNode::ConstPtr cursor = this->mBestBranch;
    while ( cursor ) {
        size_t cursorHeight = ( **cursor ).getHeight ();
        if ( cursorHeight == height ) {

            shared_ptr < Block > block = this->replaceBlock ( cursor->getBlock (), charmHex );
            this->mBestBranch = this->mBlockTree.affirmBlock ( block );
            this->composeChain ();
            return;
        }
        assert ( cursorHeight > height );
        cursor = cursor->getParent ();
    }
}

//----------------------------------------------------------------//
void SimMiner::scrambleRemotes () {

    set < shared_ptr < RemoteMiner >>::iterator remoteMinerIt = this->mOnlineMiners.begin ();
    for ( ; remoteMinerIt != this->mOnlineMiners.end (); ++remoteMinerIt ) {
        shared_ptr < RemoteMiner > remoteMiner = *remoteMinerIt;
                
        size_t height = ( **remoteMiner->mTag ).getHeight ();
        height = ( size_t )floor ( height * UnsecureRandom::get ().random ());
        
        BlockTreeNode::ConstPtr cursor = remoteMiner->mTag;
        while (( **cursor ).getHeight () > height ) {
            cursor = cursor->getParent ();
        }
        
        remoteMiner->mTag = cursor;
        remoteMiner->mHeight = height + 1;
        remoteMiner->mForward = true;
    }
}

//----------------------------------------------------------------//
SimMiner::SimMiner ( bool isGenesisMiner ) :
    mActive ( true ),
    mInterval ( 1 ),
    mIsGenesisMiner ( isGenesisMiner ) {

    this->mBlockVerificationPolicy = Block::VerificationPolicy::NONE;
}

//----------------------------------------------------------------//
SimMiner::~SimMiner () {
}

} // namespace Simulation
} // namespace Volition
