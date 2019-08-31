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

    list < shared_ptr < Transaction >>::iterator transactionIt = this->mPendingTransactions.begin ();
    for ( ; transactionIt != this->mPendingTransactions.end (); ++transactionIt ) {
        shared_ptr < Transaction > transaction = *transactionIt;
        
        // TODO: don't need to fully apply; should just check maker's nonce and sig
        if ( transaction->checkMaker ( ledger )) {
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
void Miner::extend ( bool force ) {
    
    assert ( this->mBestBranch );
    Chain& chain = *this->mBestBranch;
    
    time_t now = this->getTime ();
    
    if ( !force ) {
        shared_ptr < Block > topBlock = chain.getBlock ();
        assert ( topBlock );
        if (( this->mMinerID == topBlock->getMinerID ()) && topBlock->isInRewriteWindow ( now )) return;
    }

    size_t nBlocks = chain.countBlocks ();
    size_t top = nBlocks;
    
    for ( size_t i = nBlocks; i > 1; --i ) {
        
        // we can replace any block more recent than the current time minus the time window.
        shared_ptr < Block > rivalBlock = chain.getBlock ( i - 1 );
        if ( !rivalBlock->isInRewriteWindow ( now )) break;
        if ( rivalBlock->getMinerID () == this->mMinerID ) break; // don't overwrite own block.
        
        // prepare test block
        shared_ptr < Block > prevBlock = chain.getBlock ( i - 2 );
        Block testBlock ( this->mMinerID, now, prevBlock.get (), this->mKeyPair, Signature::DEFAULT_HASH_ALGORITHM );
        
        if ( Block::compare ( testBlock, *rivalBlock ) <= 0 ) {
            top = i - 1;
        }
    }
    
    Chain fork ( chain );
    if ( top < nBlocks ) {
        fork.revert ( top - 1 );
        fork.pushVersion ();
    }
    
    shared_ptr < Block > prevBlock = fork.getBlock ();
    assert ( prevBlock );
    
    Block block ( this->mMinerID, now, prevBlock.get (), this->mKeyPair, Signature::DEFAULT_HASH_ALGORITHM );
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
time_t Miner::getTime () const {

    return Miner_getTime ();
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
    assert ( inStream.is_open ());

    Block block;
    FromJSONSerializer::fromJSON ( block, inStream );
    this->setGenesis ( block );
}

//----------------------------------------------------------------//
void Miner::loadKey ( string keyfile, string password ) {
    UNUSED ( password );

    // TODO: password

    fstream inStream;
    inStream.open ( keyfile, ios_base::in );
    assert ( inStream.is_open ());
    
    Volition::FromJSONSerializer::fromJSON ( this->mKeyPair, inStream );
}

//----------------------------------------------------------------//
void Miner::pushTransaction ( shared_ptr < Transaction > transaction ) {

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
    mLazy ( false ) {
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
    UNUSED ( persistence );

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
    bool result = chain->pushBlock ( block );
    assert ( result );
    
    string identity = chain->getIdentity ();
    assert ( identity.size ());
    
    this->mBranches.insert ( chain );
    this->mBestBranch = chain;
}

//----------------------------------------------------------------//
void Miner::selectBranch () {
   
    shared_ptr < Chain > bestBranch;
   
    if ( this->mBranches.size ()) {
    
        time_t now = this->getTime ();
    
        set < shared_ptr < Chain >>::iterator branchIt = this->mBranches.begin ();
        bestBranch = *branchIt;
        
        for ( ++branchIt ; branchIt != this->mBranches.end (); ++branchIt ) {
            shared_ptr < Chain > comp = *branchIt;
            
            if ( Chain::compare ( *bestBranch, *comp, now ) > 0 ) {
                bestBranch = comp;
            }
        }
    }
    this->mBestBranch = bestBranch;
    assert ( this->mBestBranch );
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
                    shared_ptr < Block > rival = chain->getBlock ( blockHeight );
                    if ( block != *rival ) {
                        shared_ptr < Chain > fork = make_shared < Chain >( *chain );
                        fork->revert ( blockHeight - 1);
                        fork->pushVersion ();
                        fork->pushBlock ( block );
                        this->mBranches.insert ( fork );
                    }
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
    if ( nBlocks > 1 ) {
        this->submitChainRecurse ( chain, nBlocks - 1 );
    }
}

//----------------------------------------------------------------//
void Miner::submitChainRecurse ( const Chain& chain, size_t blockID ) {

    shared_ptr < Block > block = chain.getBlock ( blockID );
    assert ( block );
    
    if ( blockID == 0 ) {
        // TODO: handle genesis block
        assert ( false );
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
    UNUSED ( serializer );
    
//    serializer.serialize ( "chain", this->mChain );
}

//----------------------------------------------------------------//
void Miner::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
    UNUSED ( serializer );

//    serializer.serialize ( "chain", this->mChain );
}

//----------------------------------------------------------------//
time_t Miner::Miner_getTime () const {

    time_t now;
    time ( &now );
    return now;
}

} // namespace Volition
