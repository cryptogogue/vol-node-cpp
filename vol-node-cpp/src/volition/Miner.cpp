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
// RemoteMiner
//================================================================//

//----------------------------------------------------------------//
RemoteMiner::RemoteMiner () :
    mHeight ( 0 ),
    mForward ( true ) {
}

//----------------------------------------------------------------//
RemoteMiner::~RemoteMiner () {
}

//================================================================//
// Miner
//================================================================//

//----------------------------------------------------------------//
void Miner::affirmBranchSearch ( BlockTreeNode::ConstPtr node ) {

    while ( node && node->checkStatus (( BlockTreeNode::Status )( BlockTreeNode::STATUS_NEW | BlockTreeNode::STATUS_MISSING ))) {
        this->affirmNodeSearch ( node );
        node = node->getParent ();
    }
}

//----------------------------------------------------------------//
void Miner::affirmKey ( uint keyLength, unsigned long exp ) {

    if ( !this->mKeyPair ) {
        CryptoKeyPair keyPair;
        keyPair.rsa ( keyLength, exp );
        this->setKeyPair ( keyPair );
    }
    assert ( this->mKeyPair );
}

//----------------------------------------------------------------//
void Miner::affirmMessenger () {

    if ( !this->mMessenger ) {
        this->mMessenger = make_shared < HTTPMiningMessenger >();
    }
}

//----------------------------------------------------------------//
void Miner::affirmNodeSearch ( BlockTreeNode::ConstPtr node ) {

    if ( node->checkStatus (( BlockTreeNode::Status )( BlockTreeNode::STATUS_COMPLETE | BlockTreeNode::STATUS_INVALID ))) return;

    string hash = ( **node ).getDigest ();
    
    if ( this->mSearches.find ( hash ) != this->mSearches.end ()) return; // already searching

    MinerSearchEntry& search = this->mSearches [ hash ];
    
    search.mSearchTarget = node;
    search.mSearchLimit = 0;
    
    map < string, RemoteMiner >::iterator remoteMinerIt = this->mRemoteMiners.begin ();
    for ( ; remoteMinerIt != this->mRemoteMiners.end (); ++remoteMinerIt ) {
        
        RemoteMiner& remoteMiner = remoteMinerIt->second;
        
        this->mMessenger->requestBlock ( *this, remoteMinerIt->first, remoteMiner.mURL, ( **search.mSearchTarget ).getDigest ());
        search.mSearchLimit++;
    }
}

//----------------------------------------------------------------//
void Miner::affirmVisage () {

    assert ( this->mKeyPair );
    this->mVisage = this->mKeyPair.sign ( this->mMotto, Digest::HASH_ALGORITHM_SHA256 );
}

//----------------------------------------------------------------//
bool Miner::canExtend () const {

    if ( this->mFlags & MINER_MUTE ) return false;
    if ( !( this->mBestBranch && this->mBestBranch->checkStatus ( BlockTreeNode::STATUS_COMPLETE ))) return false;

    size_t count = 0;
    
    map < string, RemoteMiner >::const_iterator minerIt = this->mRemoteMiners.cbegin ();
    for ( ; minerIt != this->mRemoteMiners.cend (); ++minerIt ) {
        const RemoteMiner& remoteMiner = minerIt->second;
        if ( remoteMiner.mTag && this->mBestBranch->isAncestorOf ( remoteMiner.mTag )) count++;
    }
    return ( count > ( this->mRemoteMiners.size () >> 1 ));
}

//----------------------------------------------------------------//
bool Miner::checkBestBranch ( string miners ) const {

    assert ( this->mChain );
    return this->mChain->checkMiners ( miners );
}

//----------------------------------------------------------------//
void Miner::composeChain () {

    if ( this->mChainTag == this->mBestBranch ) return;

    // check to see if chain tag is *behind* best branch
    if ( this->mBestBranch->isAncestorOf ( this->mChainTag )) {
        this->mChain->reset (( **this->mBestBranch ).getHeight () + 1 );
        this->mChainTag = this->mBestBranch;
        return;
    }

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
void Miner::discoverMiners () {

    set < string > miners = this->getLedger ().getMiners ();
        
    set < string >::iterator minerIt = miners.begin ();
    for ( ; minerIt != miners.end (); ++minerIt ) {
        string minerID = *minerIt;
        
        if ( minerID != this->mMinerID ) {
            AccountODBM minerODBM ( this->getLedger (), *minerIt );
            this->mRemoteMiners [ minerID ].mURL = minerODBM.mMinerInfo.get ()->getURL (); // affirm
        }
    }
}

//----------------------------------------------------------------//
void Miner::extend ( time_t now ) {

    shared_ptr < Block > block = this->prepareBlock ( now );
    if ( block ) {
    
        this->pushBlock ( block );
        
        if ( this->mFlags & MINER_VERBOSE ) {
            LGN_LOG_SCOPE ( VOL_FILTER_ROOT, INFO, "WEB: MinerActivity::runSolo () - step" );
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "WEB: height: %d", ( int )this->mChain->countBlocks ());
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "WEB.CHAIN: %s", this->mChain->printChain ().c_str ());
        }
        this->saveChain ();
        this->pruneTransactions ( *this->mChain );
    }
}

//----------------------------------------------------------------//
Ledger& Miner::getLedger () {

    assert ( this->mChain );
    return *this->mChain;
}

//----------------------------------------------------------------//
bool Miner::isLazy () const {

    return ( this->mFlags & MINER_LAZY );
}

//----------------------------------------------------------------//
void Miner::loadGenesis ( string path ) {

    fstream inStream;
    inStream.open ( path, ios_base::in );
    assert ( inStream.is_open ());

    shared_ptr < Block > block = make_shared < Block >();
    FromJSONSerializer::fromJSON ( *block, inStream );
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
    assert ( this->mKeyPair );
}

//----------------------------------------------------------------//
Miner::Miner () :
    mFlags ( DEFAULT_FLAGS ),
    mRewriteMode ( BlockTreeNode::REWRITE_NONE ),
    mRewriteWindowInSeconds ( 0 ),
    mBlockVerificationPolicy ( Block::VerificationPolicy::ALL ),
    mControlLevel ( CONTROL_NONE ) {
    
    MinerLaunchTests::checkEnvironment ();
}

//----------------------------------------------------------------//
Miner::~Miner () {
}

//----------------------------------------------------------------//
shared_ptr < Block > Miner::prepareBlock ( time_t now ) {
        
    shared_ptr < Block > prevBlock = this->mChain->getBlock ();
    assert ( prevBlock );
    
    shared_ptr < Block > block = make_shared < Block >(
        this->mMinerID,
        this->mVisage,
        now,
        prevBlock.get (),
        this->mKeyPair
    );
    this->fillBlock ( *this->mChain, *block );
    
    if ( !( this->isLazy () && ( block->countTransactions () == 0 ))) {
        block->sign ( this->mKeyPair, Digest::DEFAULT_HASH_ALGORITHM );
        return block;
    }
    return NULL;
}

//----------------------------------------------------------------//
void Miner::processResponses () {

    for ( ; this->mBlockQueue.size (); this->mBlockQueue.pop_front ()) {
    
        const BlockQueueEntry& entry = *this->mBlockQueue.front ().get ();
        
        switch ( entry.mRequest.mRequestType ) {
            
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
            
            case MiningMessengerRequest::REQUEST_HEADERS:
            case MiningMessengerRequest::REQUEST_PREV_HEADERS: {
                
                string minerID = entry.mRequest.mMinerID;
                RemoteMiner& remoteMiner = this->mRemoteMiners [ minerID ];
        
                if ( entry.mHeaders.size ()) {
                    if ( remoteMiner.mHeaderQueue.size () && ( remoteMiner.mHeaderQueue.front ()->getHeight () == ( entry.mHeaders.back ()->getHeight () + 1 ))) {
                        remoteMiner.mHeaderQueue.insert ( remoteMiner.mHeaderQueue.begin (), entry.mHeaders.begin (), entry.mHeaders.end ());
                    }
                    else {
                        remoteMiner.mHeaderQueue = entry.mHeaders;
                    }
                }
                
                if ( this->mMinerSet.find ( minerID ) != this->mMinerSet.end ()) {
                    this->mMinerSet.erase ( minerID );
                }
                break;
            }
            
            default:
                assert ( false );
                break;
        }
    }
    
    map < string, RemoteMiner >::iterator remoteMinerIt = this->mRemoteMiners.begin ();
    for ( ; remoteMinerIt != this->mRemoteMiners.end (); ++remoteMinerIt ) {
    
        RemoteMiner& remoteMiner = remoteMinerIt->second;
        
        // if 'tag' gets overwritten, 'thumb' will hang on to any nodes we might need later.
        BlockTreeNode::ConstPtr thumb = remoteMiner.mTag;
        
        // visit each header in the cache and try to apply it.
        size_t accepted = 0;
        while ( remoteMiner.mHeaderQueue.size ()) {

            shared_ptr < const BlockHeader > header = *remoteMiner.mHeaderQueue.begin ();
            BlockTreeNode::ConstPtr node = this->mBlockTree.affirmBlock ( header, NULL );
            
            if ( !node ) {
                if ( accepted > 0 ) {
                    // the queue is supposed to be sequential; if part of the queue has already been
                    // accepted, then there's an error in the queue, so clear it.
                    remoteMiner.mHeaderQueue.clear ();
                }
                break;
            }
            
            // header found a parent and was added, so accept it and remove it from the cache.
            remoteMiner.mTag = node;
            remoteMiner.mHeaderQueue.pop_front ();
            accepted++;
        }
        
        if ( remoteMiner.mHeaderQueue.size ()) {
            // if there's anything left in the queue, back up and get an earlier batch of blocks.
            remoteMiner.mHeight = remoteMiner.mHeaderQueue.front ()->getHeight ();
            remoteMiner.mForward = false;
        }
        else if ( remoteMiner.mTag ) {
            // nothing in the queue, so get the next batch of blocks.
            remoteMiner.mHeight = ( **remoteMiner.mTag ).getHeight () + 1; // this doesn't really matter.
            remoteMiner.mForward = true;
        }
        else {
            // nothing at all, so get the first batch of blocks.
            remoteMiner.mHeight = 0; // doesn't matter.
            remoteMiner.mForward = true;
        }
    }
}

//----------------------------------------------------------------//
void Miner::pushBlock ( shared_ptr < const Block > block ) {

    bool result = this->mChain->pushBlock ( *block, this->mBlockVerificationPolicy );
    assert ( result );
    
    BlockTreeNode::ConstPtr node = this->mBlockTree.affirmBlock ( block );
    assert ( node );
    
    if ( this->mChainTag == this->mBestBranch ) {
        this->mBestBranch = node;
    }
    this->mChainTag = node;
}

//----------------------------------------------------------------//
void Miner::requestHeaders () {

    this->affirmMessenger ();
    
    map < string, RemoteMiner >::iterator remoteMinerIt = this->mRemoteMiners.begin ();
    for ( ; remoteMinerIt != this->mRemoteMiners.end (); ++remoteMinerIt ) {
    
        RemoteMiner& remoteMiner = remoteMinerIt->second;
        
        // constantly refill active set
        if ( this->mMinerSet.find ( remoteMinerIt->first ) == this->mMinerSet.end ()) {
            this->mMessenger->requestHeader ( *this, remoteMinerIt->first, remoteMiner.mURL, remoteMiner.mHeight, remoteMiner.mForward );
            this->mMinerSet.insert ( remoteMinerIt->first );
        }
    }
}

//----------------------------------------------------------------//
void Miner::reset () {

    this->TransactionQueue::reset ();
    this->mChain->reset ( 1 );
    this->mChain->clearSchemaCache ();
    if ( this->mChainRecorder ) {
        this->mChainRecorder->reset ();
    }
    this->Miner_reset ();
}

//----------------------------------------------------------------//
void Miner::saveChain () {

    if ( this->mChainRecorder ) {
        this->mChainRecorder->saveChain ( *this );
    }
}

//----------------------------------------------------------------//
void Miner::saveConfig () {

    if ( this->mChainRecorder ) {
        this->mChainRecorder->saveConfig ( this->mConfig );
    }
}

//----------------------------------------------------------------//
void Miner::selectBestBranch ( time_t now ) {

    BlockTreeNode::ConstPtr bestBranch = this->mBestBranch->trim (( BlockTreeNode::Status )( BlockTreeNode::STATUS_MISSING )); // best branch cannot be missing.

    map < string, RemoteMiner >::const_iterator remoteMinerIt = this->mRemoteMiners.begin ();
    for ( ; remoteMinerIt != this->mRemoteMiners.end (); ++remoteMinerIt ) {
        const RemoteMiner& remoteMiner = remoteMinerIt->second;

        if ( !remoteMiner.mTag ) continue;

        BlockTreeNode::ConstPtr truncated = this->truncate (
            remoteMiner.mTag->trim (( BlockTreeNode::Status )( BlockTreeNode::STATUS_MISSING )),
            now
        );
        
        if ( BlockTreeNode::compare ( truncated, bestBranch, this->mRewriteMode, this->mRewriteWindowInSeconds ) < 0 ) {
            bestBranch = truncated;
        }
    }
    assert ( bestBranch );
    this->mBestBranch = bestBranch;
}

//----------------------------------------------------------------//
void Miner::setChainRecorder ( shared_ptr < AbstractChainRecorder > chainRecorder ) {

    this->mChainRecorder = chainRecorder;
    if ( this->mChainRecorder ) {
        this->mChainRecorder->loadChain ( *this );
        this->mChainRecorder->loadConfig ( this->mConfig );
    }
}

//----------------------------------------------------------------//
void Miner::setGenesis ( shared_ptr < const Block > block ) {
    
    this->mChainTag = NULL;
    this->mBestBranch = NULL;
    
    assert ( block );
    
    shared_ptr < Ledger > chain = make_shared < Ledger >();
    this->mChain = chain;
    
    this->pushBlock ( block );
}

//----------------------------------------------------------------//
void Miner::setLazy ( bool lazy ) {

    this->mFlags = SET_BITS ( this->mFlags, MINER_LAZY, lazy );
}

//----------------------------------------------------------------//
void Miner::setMinimumGratuity ( u64 minimumGratuity ) {

    this->mConfig.mMinimumGratuity = minimumGratuity;
    this->saveConfig ();
}

//----------------------------------------------------------------//
void Miner::setMute ( bool paused ) {

    this->mFlags = SET_BITS ( this->mFlags, MINER_MUTE, paused );
}

//----------------------------------------------------------------//
void Miner::setReward ( string reward ) {

    this->mConfig.mReward = reward;
    this->saveConfig ();
}

//----------------------------------------------------------------//
void Miner::setRewriteWindow ( time_t window ) {

    this->mRewriteWindowInSeconds = window;
    this->setRewriteMode ( BlockTreeNode::REWRITE_WINDOW );
}

//----------------------------------------------------------------//
void Miner::setVerbose ( bool verbose ) {

    this->mFlags = SET_BITS ( this->mFlags, MINER_VERBOSE, verbose );
}

//----------------------------------------------------------------//
void Miner::shutdown ( bool kill ) {

    this->Miner_shutdown ( kill );
}

//----------------------------------------------------------------//
void Miner::step ( time_t now ) {

    Poco::ScopedLock < Poco::Mutex > scopedLock ( this->mMutex );

    this->processTransactions ();
    
    BlockTreeNode::ConstPtr prevChain = this->mChainTag;
    
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
    
    if ( this->mChainTag != prevChain ) {
        this->saveChain ();
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
        if ( remoteMiner.mTag && remoteMiner.mTag->checkStatus ( BlockTreeNode::STATUS_MISSING )) {
        
            BlockTreeNode::ConstPtr truncated = this->truncate ( remoteMiner.mTag, now );
        
            // only affirm a search if the other chain could beat our current.
            if ( BlockTreeNode::compare ( truncated, this->mBestBranch, this->mRewriteMode, this->mRewriteWindowInSeconds ) < 0 ) {
                this->affirmBranchSearch ( truncated );
            }
        }
    }
    // always affirm a search for the current branch
    this->affirmBranchSearch ( this->mBestBranch );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Miner::AbstractMiningMessengerClient_receiveBlock ( const MiningMessengerRequest& request, shared_ptr < const Block > block ) {
    
    unique_ptr < BlockQueueEntry > blockQueueEntry = make_unique < BlockQueueEntry >();
    blockQueueEntry->mRequest           = request;
    blockQueueEntry->mBlock             = block;

    Poco::ScopedLock < Poco::Mutex > chainMutexLock ( this->mMutex );
    this->mBlockQueue.push_back ( move ( blockQueueEntry ));
}

//----------------------------------------------------------------//
void Miner::AbstractMiningMessengerClient_receiveHeaders ( const MiningMessengerRequest& request, const list < shared_ptr < const BlockHeader >>& headers ) {
    
    unique_ptr < BlockQueueEntry > blockQueueEntry = make_unique < BlockQueueEntry >();
    blockQueueEntry->mRequest           = request;
    blockQueueEntry->mHeaders           = headers;

    Poco::ScopedLock < Poco::Mutex > chainMutexLock ( this->mMutex );
    this->mBlockQueue.push_back ( move ( blockQueueEntry ));
}

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
void Miner::Miner_reset () {
}


//----------------------------------------------------------------//
void Miner::Miner_shutdown ( bool kill ) {
    UNUSED ( kill );

    // explicitly release messenger and possibly trigger shutdown
    this->mMessenger = NULL;
}

} // namespace Volition
