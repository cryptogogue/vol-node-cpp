// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Miner.h>
#include <volition/TheContext.h>
#include <volition/SyncChainTask.h>

namespace Volition {

//================================================================//
// Miner
//================================================================//

//----------------------------------------------------------------//
void Miner::addTransactions ( Chain& chain, Block& block ) {

    Ledger ledger;
    ledger.takeSnapshot ( chain );

    list < shared_ptr < AbstractTransaction >>::iterator transactionIt = this->mPendingTransactions.begin ();
    for ( ; transactionIt != this->mPendingTransactions.end (); ++transactionIt ) {
        shared_ptr < AbstractTransaction > transaction = *transactionIt;
        
        // TODO: don't need to fully apply; should just check nonce and then sig
        if ( transaction->verify ( ledger )) {
            block.pushTransaction ( transaction );
        }
    }
}

//----------------------------------------------------------------//
bool Miner::checkBestBranch ( string miners ) const {

    assert ( this->mBestBranch );
    return this->mBestBranch->checkMiners ( miners );
}

//----------------------------------------------------------------//
size_t Miner::countBranches () const {

    return this->mBranches.size ();
}

//----------------------------------------------------------------//
void Miner::extend () {
    
    set < shared_ptr < Chain >>::iterator branchIt = this->mBranches.begin ();
    for ( ; branchIt != this->mBranches.end (); ++branchIt ) {
        shared_ptr < Chain > chain = *branchIt;
        this->extendChain ( *chain );
    }
}

//----------------------------------------------------------------//
void Miner::extendChain ( Chain& chain ) {

    u64 minTime = this->mNow - TheContext::get ().getWindow ();

    size_t nBlocks = chain.countBlocks ();
    size_t top = nBlocks;
    
    for ( size_t i = nBlocks; i > 1; --i ) {
        
        // we can replace any block more recent than the current time minus the time window.
        shared_ptr < Block > rivalBlock = chain.getBlock ( i - 1 );
        if ( rivalBlock->getTime () < minTime ) break;
        if ( rivalBlock->getMinerID () == this->mMinerID ) break; // don't overwrite own block.
        
        // prepare test block
        shared_ptr < Block > prevBlock = chain.getBlock ( i - 2 );
        Block testBlock ( this->mMinerID, this->mNow, prevBlock.get (), this->mKeyPair, Signature::DEFAULT_HASH_ALGORITHM );
        
        if ( Block::compare ( testBlock, *rivalBlock ) <= 0 ) {
            top = i - 1;
        }
    }
    
    Chain fork ( chain );
    if ( top < nBlocks ) {
        fork.revert ( top - 1 );
        fork.pushVersion ();
    }
    
    shared_ptr < Block > prevBlock = fork.getTopBlock ();
    assert ( prevBlock );
    
    Block block ( this->mMinerID, this->mNow, prevBlock.get (), this->mKeyPair, Signature::DEFAULT_HASH_ALGORITHM );
    this->addTransactions ( fork, block );
    
    if ( !( this->mLazy && ( block.countTransactions () == 0 ))) {
    
        block.sign ( this->mKeyPair, Signature::DEFAULT_HASH_ALGORITHM );
        bool result = fork.pushBlock ( block );
        assert ( result );
        
        chain.takeSnapshot ( fork );
    }
}

//----------------------------------------------------------------//
const Chain* Miner::getBestBranch () const {

    return this->mBestBranch.get ();
}

//----------------------------------------------------------------//
size_t Miner::getLongestBranchSize () const {
    
    size_t max = 0;
    
    set < shared_ptr < Chain >>::const_iterator branchIt = this->mBranches.cbegin ();
    for ( ; branchIt != this->mBranches.cend (); ++branchIt ) {
        shared_ptr < const Chain > comp = *branchIt;
        
        size_t nBlocks = comp->countBlocks ();
        if ( max < nBlocks ) {
            max = nBlocks;
        }
    }
    return max;
}

//----------------------------------------------------------------//
bool Miner::getLazy () const {

    return this->mLazy;
}

//----------------------------------------------------------------//
const Ledger& Miner::getLedger () const {

    assert ( this->mBestBranch );
    return *this->mBestBranch;
}

//----------------------------------------------------------------//
string Miner::getMinerID () const {

    return this->mMinerID;
}

//----------------------------------------------------------------//
bool Miner::hasBranch ( string miners ) const {

    set < shared_ptr < Chain >>::const_iterator branchIt = this->mBranches.cbegin ();
    for ( ; branchIt != this->mBranches.cend (); ++branchIt ) {
        shared_ptr < const Chain > branch = *branchIt;
        if ( branch->checkMiners ( miners )) return true;
    }
    return false;
}

//----------------------------------------------------------------//
void Miner::loadGenesis ( string path ) {

    fstream inStream;
    inStream.open ( path, ios_base::in );

    Block block;
    FromJSONSerializer::fromJSON ( block, inStream );

    this->setGenesis ( block );
}

//----------------------------------------------------------------//
void Miner::loadKey ( string keyfile, string password ) {

    // TODO: password

    fstream inStream;
    inStream.open ( keyfile, ios_base::in );
    Volition::FromJSONSerializer::fromJSON ( this->mKeyPair, inStream );
}

//----------------------------------------------------------------//
void Miner::pushTransaction ( shared_ptr < AbstractTransaction > transaction ) {

    this->mPendingTransactions.push_back ( transaction );
}

//----------------------------------------------------------------//
void Miner::setLazy ( bool lazy ) {

    this->mLazy = lazy;
}

//----------------------------------------------------------------//
void Miner::setMinerID ( string minerID ) {

    this->mMinerID = minerID;
}

//----------------------------------------------------------------//
Miner::Miner () :
    mLazy ( false ),
    mNow ( 0 ) {
}

//----------------------------------------------------------------//
Miner::~Miner () {
}

//----------------------------------------------------------------//
void Miner::saveChain () {

//    if ( this->mPersistenceProvider ) {
//        this->mChain.persist ( this->mPersistenceProvider, MASTER_BRANCH );
//        this->mPersistenceProvider->flush ();
//    }
}

//----------------------------------------------------------------//
void Miner::setPersistenceProvider ( shared_ptr < AbstractPersistenceProvider > persistence ) {

//    this->mPersistenceProvider = persistence;
//    if ( persistence ) {
//        this->mChain.takeSnapshot ( persistence, MASTER_BRANCH );
//    }
//    else {
//        this->mBranches;
//    }
}

//----------------------------------------------------------------//
void Miner::setGenesis ( const Block& block ) {
    
    this->mBranches.clear ();
    shared_ptr < Chain > chain = make_shared < Chain >();
    chain->pushBlock ( block );
    this->mBranches.insert ( chain );
    
    this->mBestBranch = chain;
}

//----------------------------------------------------------------//
void Miner::selectBranch () {
   
    shared_ptr < Chain > bestBranch;
   
    if ( this->mBranches.size ()) {
    
        set < shared_ptr < Chain >>::iterator branchIt = this->mBranches.begin ();
        bestBranch = *branchIt;
        
        for ( ++branchIt ; branchIt != this->mBranches.end (); ++branchIt ) {
            shared_ptr < Chain > comp = *branchIt;
            
            if ( Chain::compare ( *bestBranch, *comp, this->mNow, TheContext::get ().getWindow ()) > 0 ) {
                bestBranch = comp;
            }
        }
    }
    this->mBestBranch = bestBranch;
    assert ( this->mBestBranch );
}

//----------------------------------------------------------------//
void Miner::setTime ( u64 time ) {

    this->mNow = time;
}

//----------------------------------------------------------------//
void Miner::step ( u64 step ) {

    this->mNow += step;
}

//----------------------------------------------------------------//
Miner::SubmissionResponse Miner::submitBlock ( const Block& block ) {

    size_t blockHeight = block.getHeight ();
    
    assert ( blockHeight > 0 ); // TODO: handle this

    set < shared_ptr < Chain >>::iterator branchIt = this->mBranches.begin ();
    for ( ; branchIt != this->mBranches.end (); ++branchIt ) {
    
        shared_ptr < Chain > chain = *branchIt;
        size_t chainHeight = chain->countBlocks ();
        if ( blockHeight <= chainHeight ) {
            
            shared_ptr < Block > prevBlock = chain->getBlock ( blockHeight - 1 );
            assert ( prevBlock );
            if ( prevBlock->isParent ( block )) {
                
                if ( blockHeight == chainHeight ) {
                    chain->pushBlock ( block );
                }
                else {
                    shared_ptr < Chain > fork = make_shared < Chain >( *chain );
                    fork->revert ( blockHeight );
                    fork->pushBlock ( block );
                    this->mBranches.insert ( fork );
                }
                return SubmissionResponse::ACCEPTED;
            }
        }
    }
    return SubmissionResponse::RESUBMIT_EARLIER;
}

//----------------------------------------------------------------//
void Miner::submitChain ( const Chain& chain ) {

    size_t nBlocks = chain.countBlocks ();
    if ( nBlocks > 0 ) {
        this->submitChainRecurse ( chain, nBlocks - 1 );
    }
}

//----------------------------------------------------------------//
void Miner::submitChainRecurse ( const Chain& chain, size_t blockID ) {

    shared_ptr < Block > block = chain.getBlock ( blockID );
    assert ( block );
    
    if ( blockID == 0 ) {
        // TODO: handle genesis block
        return;
    }
    
    SubmissionResponse response = this->submitBlock ( *block );
    if ( response == SubmissionResponse::RESUBMIT_EARLIER ) {
        this->submitChainRecurse ( chain, blockID - 1 );
        response = this->submitBlock ( *block );
    }
    assert ( response == SubmissionResponse::ACCEPTED );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Miner::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    
//    serializer.serialize ( "chain", this->mChain );
}

//----------------------------------------------------------------//
void Miner::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

//    serializer.serialize ( "chain", this->mChain );
}

} // namespace Volition
