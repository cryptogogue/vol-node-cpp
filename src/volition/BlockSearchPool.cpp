// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/BlockSearchPool.h>
#include <volition/Digest.h>
#include <volition/FileSys.h>
#include <volition/HTTPMiningMessenger.h>
#include <volition/InMemoryBlockTree.h>
#include <volition/Miner.h>
#include <volition/MinerLaunchTests.h>
#include <volition/SQLiteBlockTree.h>
#include <volition/SQLiteConsensusInspector.h>
#include <volition/Transaction.h>
#include <volition/Transactions.h>
#include <volition/UnsecureRandom.h>

namespace Volition {

//================================================================//
// BlockSearch
//================================================================//

//----------------------------------------------------------------//
BlockSearch::BlockSearch () :
    mRetries ( 0 ),
    mT0 ( chrono::high_resolution_clock::now ()) {
}

//----------------------------------------------------------------//
bool BlockSearch::extendSearch ( BlockSearchPool& pool ) {

    Miner& miner = pool.mMiner;

    if ( SAMPLE_SIZE <= this->mActiveMiners.size ()) return true;

    BlockTreeCursor cursor = miner.mBlockTree->findCursorForHash ( this->mHash );

    // get all the miners that are active and not already in searches
    map < string, shared_ptr < RemoteMiner >> remoteMinersByName;
    set < shared_ptr < RemoteMiner >>::iterator remoteMinerIt = miner.mRemoteMiners.begin ();
    for ( ; remoteMinerIt != miner.mRemoteMiners.end (); ++remoteMinerIt ) {
    
        shared_ptr < RemoteMiner > remoteMiner = *remoteMinerIt;
        if ( !remoteMiner->isOnline ()) continue;
        
        string minerID = remoteMiner->getMinerID ();
        
        if ( this->mCompletedMiners.find ( minerID ) != this->mCompletedMiners.end ()) continue;
        if ( this->mActiveMiners.find ( minerID ) != this->mActiveMiners.end ()) continue;
        
        remoteMinersByName [ remoteMiner->getMinerID ()] = remoteMiner;
    }

    size_t sampleSize = ( SAMPLE_SIZE - this->mActiveMiners.size ());
    
    for ( size_t i = 0; (( i < sampleSize ) && ( remoteMinersByName.size () > 0 )); ++i ) {
    
        // if the block's miner is online, make sure that miner is in the first batch of searches.
        map < string, shared_ptr < RemoteMiner >>::iterator remoteMinerByNameIt = remoteMinersByName.find ( cursor.getMinerID ());
        if ( remoteMinerByNameIt == remoteMinersByName.end ()) {
            remoteMinerByNameIt = remoteMinersByName.begin ();
            advance ( remoteMinerByNameIt, ( long )( UnsecureRandom::get ().random ( 0, remoteMinersByName.size () - 1 )));
        }
        
        shared_ptr < RemoteMiner > remoteMiner = remoteMinerByNameIt->second;
        remoteMinersByName.erase ( remoteMinerByNameIt );
        
        this->mActiveMiners.insert ( remoteMiner->getMinerID ());
        miner.mMessenger->enqueueBlockRequest (
            remoteMiner->mURL,
            cursor.getDigest (),
            cursor.getHeight (),
            Format::write ( "%s:%s", miner.mMinerID.c_str (), cursor.getCharmTag ().c_str ())
        );
    }
    
    return ( this->mActiveMiners.size () > 0 );
}

//----------------------------------------------------------------//
void BlockSearch::initialize ( BlockTreeCursor cursor ) {

    this->mTag      = cursor.write ();
    this->mHeight   = cursor.getHeight ();
    this->mHash     = cursor.getHash ();
}

//----------------------------------------------------------------//
void BlockSearch::reset () {

    this->mCompletedMiners.clear ();
}

//----------------------------------------------------------------//
bool BlockSearch::step ( BlockSearchPool& pool ) {

    Miner& miner = pool.mMiner;
    BlockTreeCursor cursor = miner.mBlockTree->findCursorForHash ( this->mHash );
    
    if ( cursor.hasBlock ()) return false;
    if ( !( cursor.hasHeader () && cursor.checkStatus (( kBlockTreeEntryStatus )( kBlockTreeEntryStatus::STATUS_NEW | kBlockTreeEntryStatus::STATUS_MISSING )))) return false;
    
    if ( miner.mRemoteMiners.size () == 0 ) return true;

    if ( this->extendSearch ( pool )) return true;
    
    if ( this->mRetries < MAX_RETRIES ) {
        this->reset ();
        this->mRetries++;
        if ( this->extendSearch ( pool )) return true;
    }
    
    miner.mBlockTree->mark ( cursor, kBlockTreeEntryStatus::STATUS_MISSING );
    return false;
}

//----------------------------------------------------------------//
void BlockSearch::step ( string minerID ) {

    assert ( this->mActiveMiners.find ( minerID ) != this->mActiveMiners.cend ());
    this->mCompletedMiners.insert ( minerID );
    this->mActiveMiners.erase ( minerID );
}

//================================================================//
// BlockSearchPool
//================================================================//

//----------------------------------------------------------------//
void BlockSearchPool::affirmBlockSearch ( BlockTreeCursor cursor ) {

    if ( cursor.hasBlock ()) return;

    string hash = cursor.getDigest ();
    if ( this->mBlockSearchesByHash.find ( hash ) != this->mBlockSearchesByHash.end ()) return; // already searching

    BlockSearch& search = this->mBlockSearchesByHash [ hash ];
    search.initialize ( cursor );
    
    this->mPendingSearches.insert ( search );
}

//----------------------------------------------------------------//
void BlockSearchPool::affirmBranchSearch ( BlockTreeCursor cursor ) {

    while ( !cursor.isComplete ()) {
    
        if ( !( cursor.hasHeader () && cursor.checkStatus (( kBlockTreeEntryStatus )( kBlockTreeEntryStatus::STATUS_NEW | kBlockTreeEntryStatus::STATUS_MISSING )))) return;
        
        bool skip = cursor.hasBlock () || ( cursor.isProvisional () && ( cursor.getMinerID () == this->mMiner.getMinerID()));
        
        if ( !skip ) {
            this->affirmBlockSearch ( cursor );
        }
        cursor = cursor.getParent ();
    }
}

//----------------------------------------------------------------//
BlockSearchPool::BlockSearchPool ( Miner& miner ) :
    mMiner ( miner ) {
}

//----------------------------------------------------------------//
BlockSearchPool::~BlockSearchPool () {
}

//----------------------------------------------------------------//
void BlockSearchPool::erase ( string hash ) {

    map < string, BlockSearch >::iterator blockSearchIt = this->mBlockSearchesByHash.find ( hash );
    if ( blockSearchIt != this->mBlockSearchesByHash.end ()) {
    
        BlockSearchKey key ( blockSearchIt->second );
        
        this->mActiveSearches.erase ( key );
        this->mPendingSearches.erase ( key );
        this->mBlockSearchesByHash.erase ( hash );
    }
}

//----------------------------------------------------------------//
BlockSearch* BlockSearchPool::findBlockSearch ( const Digest& digest ) {

    map < string, BlockSearch >::iterator searchIt = this->mBlockSearchesByHash.find ( digest.toHex ());
    if ( searchIt == this->mBlockSearchesByHash.cend ()) return NULL; // no search; bail.
    
    return &searchIt->second;
}

//----------------------------------------------------------------//
void BlockSearchPool::reportBlockSearches () const {

    LGN_LOG_SCOPE ( VOL_FILTER_MINING_SEARCH_REPORT, INFO, __PRETTY_FUNCTION__ );

     set < BlockSearchKey >::const_iterator blockSearchIt = this->mActiveSearches.begin ();
     for ( ; blockSearchIt != this->mActiveSearches.end (); ++blockSearchIt ) {
        const BlockSearch& blockSearch = blockSearchIt->mBlockSearch;
        
        LGN_LOG ( VOL_FILTER_MINING_SEARCH_REPORT, INFO, "BLOCK SEARCH: %s", blockSearch.mTag.c_str ());
        
        LGN_LOG ( VOL_FILTER_MINING_SEARCH_REPORT, INFO, "    HASH: %s", blockSearch.mHash.c_str ());
        
        set < string >::const_iterator activeIt = blockSearch.mActiveMiners.begin ();
        for ( ; activeIt != blockSearch.mActiveMiners.end (); ++activeIt ) {
            LGN_LOG ( VOL_FILTER_MINING_SEARCH_REPORT, INFO, "    ACTIVE: %s", activeIt->c_str ());
        }
        
        set < string >::const_iterator completedIt = blockSearch.mCompletedMiners.begin ();
        for ( ; completedIt != blockSearch.mCompletedMiners.end (); ++completedIt ) {
            LGN_LOG ( VOL_FILTER_MINING_SEARCH_REPORT, INFO, "    DONE: %s", completedIt->c_str ());
        }
        
        chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now ();
        chrono::milliseconds span = chrono::duration_cast < chrono::milliseconds >( t1 - blockSearch.mT0 );
        
        LGN_LOG ( VOL_FILTER_MINING_SEARCH_REPORT, INFO, "    TIME: %.2lfs", ( double )span.count () * 0.001 );
        
        LGN_LOG ( VOL_FILTER_MINING_SEARCH_REPORT, INFO, "    RETRIES: %d", ( int )blockSearch.mRetries );
    }
}

//----------------------------------------------------------------//
void BlockSearchPool::update () {

    LGN_LOG_SCOPE ( VOL_FILTER_CONSENSUS, INFO, __PRETTY_FUNCTION__ );
    
    while (( this->mActiveSearches.size () < MAX_SEARCHES ) && ( this->mPendingSearches.size ())) {
    
        set < BlockSearchKey >::iterator next = this->mPendingSearches.begin ();
        BlockSearchKey key = *next;
        
        this->mActiveSearches.insert ( key );
        this->mPendingSearches.erase ( next );
    }
    
    // step the currently active block searches
    set < BlockSearchKey >::iterator blockSearchIt = this->mActiveSearches.begin ();
    while ( blockSearchIt != this->mActiveSearches.end ()) {
        
        set < BlockSearchKey >::iterator cursor = blockSearchIt++;
        
        BlockSearch& blockSearch = cursor->mBlockSearch;
        if ( !blockSearch.step ( *this )) {
            this->erase ( blockSearch.mHash );
        }
    }
}

//----------------------------------------------------------------//
void BlockSearchPool::updateBlockSearch ( string minerID, string hash, bool found ) {

    map < string, BlockSearch >::iterator blockSearchIt = this->mBlockSearchesByHash.find ( hash );
    if ( blockSearchIt != this->mBlockSearchesByHash.end ()) {
        if ( found ) {
            this->erase ( blockSearchIt->first );
        }
        else {
            blockSearchIt->second.step ( minerID );
        }
    }
}

} // namespace Volition
