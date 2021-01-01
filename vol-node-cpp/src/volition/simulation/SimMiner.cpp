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

    shared_ptr < const Block > prevBlock = this->mWorkingLedger->getBlock ();

    shared_ptr < Block > block = make_shared < Block >();
    block->initialize (
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
    this->mPermanentLedgerTag = this->mWorkingLedgerTag;
    this->mPermanentLedger = *this->mWorkingLedger;
}

//----------------------------------------------------------------//
shared_ptr < Block > SimMiner::replaceBlock ( shared_ptr < const Block > oldBlock, string charmHex ) {
    
    assert ( oldBlock );
    
    BlockTreeCursor prevNode = (( const BlockTree& )this->mBlockTree ).findCursorForHash ( oldBlock->getPrevDigest ());
    assert ( prevNode.hasHeader ());
    
    shared_ptr < const Block > prevBlock = prevNode.getBlock ();
    assert ( prevBlock );
    
    shared_ptr < Block > block = make_shared < Block >();
    block->initialize (
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

    while ( this->mBestProvisional.getHeight () > height ) {
        this->mBestProvisional = ( *this->mBestProvisional ).getParent ();
    }
    this->composeChain ();
}

//----------------------------------------------------------------//
void SimMiner::setActive ( bool active ) {

    this->mActive = active;
}

//----------------------------------------------------------------//
void SimMiner::setCharm ( size_t height, string charmHex ) {

    BlockTreeCursor cursor = *this->mBestProvisional;
    while ( cursor.hasHeader ()) {
        size_t cursorHeight = cursor.getHeight ();
        if ( cursorHeight == height ) {

            shared_ptr < Block > block = this->replaceBlock ( cursor.getBlock (), charmHex );
            this->mBlockTree.affirmBlock ( this->mBestProvisional, block );
            this->composeChain ();
            return;
        }
        assert ( cursorHeight > height );
        cursor = cursor.getParent ();
    }
}

//----------------------------------------------------------------//
void SimMiner::scrambleRemotes () {

    set < shared_ptr < RemoteMiner >>::iterator remoteMinerIt = this->mOnlineMiners.begin ();
    for ( ; remoteMinerIt != this->mOnlineMiners.end (); ++remoteMinerIt ) {
        shared_ptr < RemoteMiner > remoteMiner = *remoteMinerIt;
                
        size_t height = remoteMiner->mTag.getHeight ();
        height = ( size_t )floor ( height * UnsecureRandom::get ().random ());
        
        BlockTreeCursor cursor = *remoteMiner->mTag;
        while ( cursor.getHeight () > height ) {
            cursor = cursor.getParent ();
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
