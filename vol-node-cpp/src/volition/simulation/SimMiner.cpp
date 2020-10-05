// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/simulation/SimMiner.h>

namespace Volition {
namespace Simulation {

//================================================================//
// SimMiner
//================================================================//

//----------------------------------------------------------------//
shared_ptr < Block > SimMiner::replaceBlock ( shared_ptr < const Block > oldBlock, string charmHex ) {
    
    assert ( oldBlock );
    
    BlockTreeNode::ConstPtr prevNode = (( const BlockTree& )this->mBlockTree ).findNodeForHash ( oldBlock->getPrevHash ());
    assert ( prevNode );
    
    shared_ptr < const Block > prevBlock = prevNode->getBlock ();
    assert ( prevBlock );
    
    shared_ptr < Block > block = make_shared < Block >( this->mMinerID, this->mVisage, oldBlock->getTime (), prevBlock.get (), this->mKeyPair );
    
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

    BlockTreeNode::ConstPtr cursor = this->mBestBranch;
    while (( **cursor ).getHeight () > height ) {
        cursor = cursor->getParent ();
    }
    if (( **cursor ).getHeight () == height ) {
        this->mChain->reset (( **cursor ).getHeight () + 1 );
        this->mChainTag     = cursor;
        this->mBestBranch   = cursor;
    }
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
SimMiner::SimMiner () {

    this->mBlockVerificationPolicy = Block::VerificationPolicy::NONE;
}

//----------------------------------------------------------------//
SimMiner::~SimMiner () {
}

} // namespace Simulation
} // namespace Volition
