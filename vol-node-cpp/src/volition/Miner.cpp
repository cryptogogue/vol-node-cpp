// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractChainRecorder.h>
#include <volition/Block.h>
#include <volition/Digest.h>
#include <volition/HTTPMiningMessenger.h>
#include <volition/Miner.h>
#include <volition/MinerLaunchTests.h>

namespace Volition {

//================================================================//
// Miner
//================================================================//

//----------------------------------------------------------------//
void Miner::affirmSearch ( BlockTreeNode::ConstPtr branch ) {

    if ( branch->checkStatus (( BlockTreeNode::Status )( BlockTreeNode::STATUS_COMPLETE | BlockTreeNode::STATUS_INVALID ))) return;

    string hash = ( **branch ).getDigest ();
    
    if ( this->mSearches.find ( hash ) != this->mSearches.end ()) return; // already searching

    MinerSearchEntry& search = this->mSearches [ hash ];
    
    search.mSearchTarget = branch;
    search.mSearchLimit = 0;
    
    map < string, RemoteMiner >::iterator remoteMinerIt = this->mRemoteMiners.begin ();
    for ( ; remoteMinerIt != this->mRemoteMiners.end (); ++remoteMinerIt ) {
        
        RemoteMiner& remoteMiner = remoteMinerIt->second;
        
        this->mMessenger->requestBlock ( *this, remoteMinerIt->first, remoteMiner.mURL, ( **search.mSearchTarget ).getDigest ());
        search.mSearchLimit++;
    }
}

//----------------------------------------------------------------//
bool Miner::canExtend () const {

    if ( !( this->mBestBranch && this->mBestBranch->checkStatus ( BlockTreeNode::STATUS_COMPLETE ))) return false;

    size_t count = 0;
    
    map < string, RemoteMiner >::const_iterator minerIt = this->mRemoteMiners.cbegin ();
    for ( ; minerIt != this->mRemoteMiners.cend (); ++minerIt ) {
        const RemoteMiner& remoteMiner = minerIt->second;
        if ( this->mBestBranch->isAncestorOf ( remoteMiner.mTag )) count++;
    }
    return ( count > ( this->mRemoteMiners.size () >> 1 ));
}

//----------------------------------------------------------------//
void Miner::composeChain () {

    // if chain is divergent from best branch, re-root it
    if ( !this->mChainTag->isAncestorOf ( this->mBestBranch )) {
                    
        // REWIND chain to point of divergence
        BlockTreeNode::ConstPtr root = BlockTreeNode::findRoot ( this->mChainTag, this->mBestBranch ).mRoot;
        assert ( root ); // guaranteed -> common genesis
        assert ( root->checkStatus ( BlockTreeNode::STATUS_COMPLETE ));  // guaranteed -> was in chain
        
        this->mChain->reset (( **root ).getHeight () + 1 );
        this->mChainTag = root;
    }
    assert ( this->mChainTag->isAncestorOf ( this->mBestBranch ));
    
    this->updateChainRecurse ( this->mBestBranch );
}

//----------------------------------------------------------------//
Miner::Miner () {
}

//----------------------------------------------------------------//
Miner::~Miner () {
}

//----------------------------------------------------------------//
void Miner::processResponses () {

    for ( ; this->mBlockQueue.size (); this->mBlockQueue.pop_front ()) {
    
        const BlockQueueEntry& entry = *this->mBlockQueue.front ().get ();
        string minerID = entry.mRequest.mMinerID;
        
        RemoteMiner& remoteMiner = this->mRemoteMiners [ minerID ];

        switch ( entry.mRequest.mRequestType ) {
        
            case MiningMessengerRequest::REQUEST_HEADER: {
                
                if ( entry.mBlockHeader ) {
                    remoteMiner.mTag = entry.mBlockHeader ? this->mBlockTree.affirmBlock ( entry.mBlockHeader, NULL ) : NULL;
                    remoteMiner.mCurrentBlock = remoteMiner.mTag ? entry.mRequest.mHeight + 1 : entry.mRequest.mHeight - 1;
                }
                
                if ( this->mMinerSet.find ( minerID ) != this->mMinerSet.end ()) {
                    this->mMinerSet.erase ( minerID );
                }
                break;
            }
            
            case MiningMessengerRequest::REQUEST_BLOCK: {
                
                this->mBlockTree.update ( entry.mBlock ); // update no matter what (will do nothing if node is missing)
                
                string hash = entry.mRequest.mBlockDigest;
                map < string, MinerSearchEntry >::iterator searchIt = this->mSearches.find ( hash );
                if ( searchIt == this->mSearches.end ()) break;
                
                MinerSearchEntry& search = searchIt->second;
                search.mSearchCount++;

                if (( search.mSearchCount >= search.mSearchLimit ) && search.mSearchTarget->checkStatus ( BlockTreeNode::STATUS_NEW )) {
                    this->mBlockTree.mark ( search.mSearchTarget, BlockTreeNode::STATUS_MISSING );
                }

                if ( !search.mSearchTarget->checkStatus ( BlockTreeNode::STATUS_NEW )) {
                    this->mSearches.erase ( hash );
                }
                break;
            }
            
            default:
                assert ( false );
                break;
        }
    }
}

//----------------------------------------------------------------//
void Miner::requestHeaders () {

    this->affirmMessenger ();
    
    map < string, RemoteMiner >::iterator remoteMinerIt = this->mRemoteMiners.begin ();
    for ( ; remoteMinerIt != this->mRemoteMiners.end (); ++remoteMinerIt ) {
    
        RemoteMiner& remoteMiner = remoteMinerIt->second;
        
        // constantly refill active set
        if ( this->mMinerSet.find ( remoteMinerIt->first ) == this->mMinerSet.end ()) {
            this->mMessenger->requestHeader ( *this, remoteMinerIt->first, remoteMiner.mURL, remoteMiner.mCurrentBlock );
            this->mMinerSet.insert ( remoteMinerIt->first );
        }
    }
}

//----------------------------------------------------------------//
void Miner::selectBestBranch ( time_t now ) {

    map < string, RemoteMiner >::const_iterator remoteMinerIt = this->mRemoteMiners.begin ();
    for ( ; remoteMinerIt != this->mRemoteMiners.end (); ++remoteMinerIt ) {
        const RemoteMiner& remoteMiner = remoteMinerIt->second;

        if ( !remoteMiner.mTag ) continue;

        BlockTreeNode::ConstPtr truncated = this->truncate (
            remoteMiner.mTag->trim (( BlockTreeNode::Status )( BlockTreeNode::STATUS_MISSING )),
            now
        );
        
        if ( BlockTreeNode::compare ( truncated, this->mBestBranch, this->mRewriteMode, this->mRewriteWindowInSeconds ) < 0 ) {
            this->mBestBranch = truncated;
        }
    }
    assert ( this->mBestBranch );
}

//----------------------------------------------------------------//
void Miner::step ( time_t now ) {

    Poco::ScopedLock < Poco::Mutex > scopedLock ( this->mMutex );

    this->processTransactions ( *this );
    
    if ( this->mMessenger ) {
    
        // APPLY incoming blocks
        this->processResponses ();
        
        // CHOOSE new branch
        this->selectBestBranch ( now );
        
        // SEARCH for missing blocks
        this->updateSearches ( now );
        
        // BUILD the current chain
        this->composeChain ();
        
        // EXTEND chain if complete and has consensus
        if ( this->canExtend ()) {
            this->extend ( now );
        }

        // SCAN the ledger for miners
        this->discoverMiners ();
        
        // QUERY the network for headers
        this->requestHeaders ();
    }
    else {
        this->extend ( now );
    }
}

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr Miner::truncate ( BlockTreeNode::ConstPtr tail, time_t now ) const {

    if ( this->mRewriteMode == BlockTreeNode::REWRITE_NONE ) return tail;

    // if a block from self would be more charming at any point along the chain,
    // truncate the chain to the parent of that block. in other words: seek the
    // earliest insertion point for a local block. if we find a block from
    // self, abort: to truncate, our local block must *beat* any other block.

    // TODO: this should take into account the lookback window.

    BlockTreeNode::ConstPtr cursor = tail;
    
    while ( cursor ) {
    
        const BlockHeader& header = **cursor;
    
        if (( this->mRewriteMode == BlockTreeNode::REWRITE_WINDOW ) && !header.isInRewriteWindow ( this->mRewriteWindowInSeconds, now )) return tail;
    
        BlockTreeNode::ConstPtr parent = cursor->getParent ();
        if ( !parent ) break;
        
        const BlockHeader& parentHeader = **parent;
        
        if ( header.getMinerID () == this->mMinerID ) break;
        
        Digest charm = parentHeader.getNextCharm ( this->mVisage );
        if ( BlockHeader::compare ( charm, header.getCharm ()) < 0 ) return parent;
        
        cursor = parent;
    }

    return tail;
}

//----------------------------------------------------------------//
void Miner::updateChainRecurse ( BlockTreeNode::ConstPtr branch ) {

    if ( this->mChainTag == branch ) return; // nothing to do
    
    BlockTreeNode::ConstPtr parent = branch->getParent ();
    if ( parent != this->mChainTag ) {
        this->updateChainRecurse ( parent );
    }
    
    assert ( this->mChainTag == parent );
    if ( branch->checkStatus ( BlockTreeNode::STATUS_COMPLETE )) {
        this->pushBlock ( branch->getBlock ());
        assert ( this->mChainTag == branch );
    }
}

//----------------------------------------------------------------//
void Miner::updateSearches ( time_t now ) {

    map < string, RemoteMiner >::const_iterator remoteMinerIt = this->mRemoteMiners.begin ();
    for ( ; remoteMinerIt != this->mRemoteMiners.end (); ++remoteMinerIt ) {
        
        const RemoteMiner& remoteMiner = remoteMinerIt->second;

        // we only care about missing branches; ignore new/complete/invalid branches.
        if ( remoteMiner.mTag->checkStatus ( BlockTreeNode::STATUS_MISSING )) {
        
            BlockTreeNode::ConstPtr truncated = this->truncate ( remoteMiner.mTag, now );
        
            // only affirm a search if the other chang could beat our current.
            if ( BlockTreeNode::compare ( truncated, this->mBestBranch, this->mRewriteMode, this->mRewriteWindowInSeconds ) < 0 ) {
                this->affirmSearch ( truncated );
            }
        }
    }
    // always affirm a search for the current branch
    this->affirmSearch ( this->mBestBranch );
}

} // namespace Volition
