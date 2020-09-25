// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractChainRecorder.h>
#include <volition/Block.h>
#include <volition/Digest.h>
#include <volition/HTTPMiningMessenger.h>
#include <volition/Miner.h>
#include <volition/MinerLaunchTests.h>
#include <volition/TheContext.h>

namespace Volition {

//================================================================//
// RemoteMiner
//================================================================//

//----------------------------------------------------------------//
RemoteMiner::RemoteMiner () :
    mCurrentBlock ( 0 ) {
}

//----------------------------------------------------------------//
RemoteMiner::~RemoteMiner () {
}

//================================================================//
// Miner
//================================================================//

//----------------------------------------------------------------//
void Miner::affirmKey ( uint keyLength, unsigned long exp ) {

    if ( !this->mKeyPair ) {
        this->mKeyPair.rsa ( keyLength, exp );
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
void Miner::affirmVisage () {

    assert ( this->mKeyPair );
    this->mVisage = this->mKeyPair.sign ( this->mMotto, Digest::HASH_ALGORITHM_SHA256 );
}

//----------------------------------------------------------------//
bool Miner::checkBestBranch ( string miners ) const {

    assert ( this->mChain );
    return this->mChain->checkMiners ( miners );
}

//----------------------------------------------------------------//
bool Miner::controlPermitted () const {

    return this->mControlPermitted;
}

//----------------------------------------------------------------//
void Miner::discoverMiners () {

    map < string, MinerInfo > miners = this->getBestBranch ()->getMiners ();
        
    map < string, MinerInfo >::iterator minerIt = miners.begin ();
    for ( ; minerIt != miners.end (); ++minerIt ) {
        MinerInfo& minerInfo = minerIt->second;
        if ( minerIt->first != this->mMinerID ) {
            this->mRemoteMiners [ minerIt->first ].mURL = minerInfo.getURL (); // affirm
        }
    }
}

//----------------------------------------------------------------//
void Miner::extend () {

    shared_ptr < Block > block = this->prepareBlock ();
    if ( block ) {
    
        this->pushBlock ( block );
        
        if ( this->mVerbose ) {
            LGN_LOG_SCOPE ( VOL_FILTER_ROOT, INFO, "WEB: WebMiner::runSolo () - step" );
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "WEB: height: %d", ( int )this->mChain->countBlocks ());
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "WEB.CHAIN: %s", this->mChain->print ().c_str ());
        }
        this->saveChain ();
        this->pruneTransactions ( *this->mChain );
    }
}

//----------------------------------------------------------------//
const Chain* Miner::getBestBranch () const {

    return this->mChain.get ();
}

//----------------------------------------------------------------//
const BlockTree& Miner::getBlockTree () const {

    return this->mBlockTree;
}

//----------------------------------------------------------------//
shared_ptr < const BlockTreeNode > Miner::getBlockTreeTag () const {

    return this->mGoalTag;
}

//----------------------------------------------------------------//
const CryptoKey& Miner::getKeyPair () const {

    return this->mKeyPair;
}

//----------------------------------------------------------------//
bool Miner::getLazy () const {

    return this->mLazy;
}

//----------------------------------------------------------------//
Ledger& Miner::getLedger () {

    assert ( this->mChain );
    return *this->mChain;
}

//----------------------------------------------------------------//
const Ledger& Miner::getLedger () const {

    assert ( this->mChain );
    return *this->mChain;
}

//----------------------------------------------------------------//
string Miner::getMinerID () const {

    return this->mMinerID;
}

//----------------------------------------------------------------//
string Miner::getMotto () const {

    return this->mMotto;
}

//----------------------------------------------------------------//
SerializableTime Miner::getStartTime () const {

    return this->mStartTime;
}

//----------------------------------------------------------------//
time_t Miner::getTime () const {

    return this->Miner_getTime ();
}

//----------------------------------------------------------------//
const Signature& Miner::getVisage () const {

    return this->mVisage;
}

//----------------------------------------------------------------//
bool Miner::hasConsensus () const {

    if ( !( this->mGoalTag && this->mGoalTag->isComplete ())) return false;

    size_t count = 0;
    
    map < string, RemoteMiner >::const_iterator minerIt = this->mRemoteMiners.cbegin ();
    for ( ; minerIt != this->mRemoteMiners.cend (); ++minerIt ) {
        const RemoteMiner& remoteMiner = minerIt->second;
        if ( this->mGoalTag->isAncestorOf ( remoteMiner.mTag )) count++;
    }
    return ( count > ( this->mRemoteMiners.size () >> 1 ));
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
    mLazy ( false ),
    mSolo ( true ),
    mVerbose ( false ),
    mControlPermitted ( false ),
    mBlockVerificationPolicy ( Block::VerificationPolicy::ALL ),
    mSearchCount ( 0 ),
    mSearchLimit ( 0 ) {
    
    MinerLaunchTests::checkEnvironment ();
}

//----------------------------------------------------------------//
Miner::~Miner () {
}

//----------------------------------------------------------------//
void Miner::permitControl ( bool permit ) {

    this->mControlPermitted = permit;
}

//----------------------------------------------------------------//
shared_ptr < Block > Miner::prepareBlock () {
        
    shared_ptr < Block > prevBlock = this->mChain->getBlock ();
    assert ( prevBlock );
    
    shared_ptr < Block > block = make_shared < Block >( this->mMinerID, this->mVisage, this->getTime (), prevBlock.get (), this->mKeyPair );
    this->fillBlock ( *this->mChain, *block );
    
    if ( !( this->mLazy && ( block->countTransactions () == 0 ))) {
        block->sign ( this->mKeyPair, Digest::DEFAULT_HASH_ALGORITHM );
        return block;
    }
    return NULL;
}

//----------------------------------------------------------------//
void Miner::processQueue () {

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
                
                if ( entry.mBlock ) {
                    this->mBlockTree.affirmBlock ( entry.mBlock );
                }
                
                if ( this->mSearchTarget && ( entry.mRequest.mBlockDigest == ( **this->mSearchTarget ).getDigest ())) {
                    this->mSearchCount++;
                    
                    if (( this->mSearchCount >= this->mSearchLimit ) && this->mSearchTarget->isPending ()) {
                        this->mBlockTree.markExpired ( this->mSearchTarget );
                    }
                }
                
                if ( this->mSearchTarget && !this->mSearchTarget->isPending ()) {
                    this->mSearchTarget     = NULL;
                    this->mSearchCount      = 0;
                    this->mSearchLimit      = 0;
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
void Miner::pushBlock ( shared_ptr < const Block > block ) {

    bool result = this->mChain->pushBlock ( *block, this->mBlockVerificationPolicy );
    assert ( result );
    
    BlockTreeNode::ConstPtr node = this->mBlockTree.affirmBlock ( block );
    assert ( node );
    
    if ( this->mChainTag == this->mGoalTag ) {
        this->mGoalTag = node;
    }
    this->mChainTag = node;
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
void Miner::setChainRecorder ( shared_ptr < AbstractChainRecorder > chainRecorder ) {

    this->mChainRecorder = chainRecorder;
    if ( this->mChainRecorder ) {
        this->mChainRecorder->loadChain ( *this );
    }
}

//----------------------------------------------------------------//
void Miner::setGenesis ( shared_ptr < const Block > block ) {
    
    this->mChainTag = NULL;
    this->mGoalTag = NULL;
    
    assert ( block );
    
    shared_ptr < Chain > chain = make_shared < Chain >();
    this->mChain = chain;
    
    this->pushBlock ( block );
}

//----------------------------------------------------------------//
void Miner::setLazy ( bool lazy ) {

    this->mLazy = lazy;
}

//----------------------------------------------------------------//
void Miner::setMessenger ( shared_ptr < AbstractMiningMessenger > messenger ) {

    this->mMessenger = messenger;
}

//----------------------------------------------------------------//
void Miner::setMinerID ( string minerID ) {

    this->mMinerID = minerID;
}

//----------------------------------------------------------------//
void Miner::setMotto ( string motto ) {

    this->mMotto = motto;
}

//----------------------------------------------------------------//
void Miner::setSolo ( bool solo ) {

    this->mSolo = solo;
}

//----------------------------------------------------------------//
void Miner::setVerbose ( bool verbose ) {

    this->mVerbose = verbose;
}

//----------------------------------------------------------------//
void Miner::shutdown ( bool kill ) {

    this->Miner_shutdown ( kill );
}

//----------------------------------------------------------------//
void Miner::startTasks () {

    this->affirmMessenger ();
    
    bool startBlockSearch = ( this->mSearchTarget && ( this->mSearchCount == 0 ));
    if ( startBlockSearch ) {
        this->mSearchLimit = 0;
    }
    
    map < string, RemoteMiner >::iterator remoteMinerIt = this->mRemoteMiners.begin ();
    for ( ; remoteMinerIt != this->mRemoteMiners.end (); ++remoteMinerIt ) {
    
        RemoteMiner& remoteMiner = remoteMinerIt->second;
        
        // constantly refill active set
        if ( this->mMinerSet.find ( remoteMinerIt->first ) == this->mMinerSet.end ()) {
            this->mMessenger->requestHeader ( *this, remoteMinerIt->first, remoteMiner.mURL, remoteMiner.mCurrentBlock );
            this->mMinerSet.insert ( remoteMinerIt->first );
        }
        
        if ( startBlockSearch ) {
            this->mMessenger->requestBlock ( *this, remoteMinerIt->first, remoteMiner.mURL, ( **this->mSearchTarget ).getDigest ());
            this->mSearchLimit++;
        }
    }
}

//----------------------------------------------------------------//
void Miner::step ( bool solo ) {

    Poco::ScopedLock < Poco::Mutex > scopedLock ( this->mMutex );

    this->processIncoming ( *this );
        
    if ( solo ) {
        this->extend ();
    }
    else {
    
        // APPLY incoming blocks
        this->processQueue ();
        
        // PRUNE expired
        BlockTreeNode::ConstPtr originalBranch  = this->mGoalTag;
        BlockTreeNode::ConstPtr nextBranch      = this->mGoalTag;
        
        // CHOOSE new branch
        
        // we only endorse a branch if it hasn't been blacklisted.
        // if a branch is blacklisted, keep trying to find its nodes for as long
        // as the blacklisted branch could beat the current best branch.
        
        map < string, RemoteMiner >::const_iterator remoteMinerIt = this->mRemoteMiners.begin ();
        for ( ; remoteMinerIt != this->mRemoteMiners.end (); ++remoteMinerIt ) {
            const RemoteMiner& remoteMiner = remoteMinerIt->second;

            BlockTreeNode::ConstPtr truncated = this->truncate ( this->trimExpired ( remoteMiner.mTag ));
                      
            if ( truncated && ( BlockTreeNode::compare ( truncated, nextBranch ) < 0 )) {
                nextBranch = truncated;
            }
        }
        
        if ( originalBranch != nextBranch ) {
        
            this->mGoalTag = nextBranch;
            
            // REWIND chain to point of divergence
            BlockTreeNode::ConstPtr root = BlockTreeNode::findRoot ( this->mChainTag, this->mGoalTag ).mRoot;
            assert ( root );                // guaranteed -> common genesis
            assert ( root->isComplete ());  // guaranteed -> was in chain
            
            this->mChain->reset (( **root ).getHeight () + 1 );
            this->mChainTag = root;
            
            // TODO: this is a bit gratuitous; can do better than a full rebuld each time
            this->mNodeQueue.clear ();
            for ( BlockTreeNode::ConstPtr cursor = this->mGoalTag; cursor != this->mChainTag; cursor = cursor->getParent ()) {
                this->mNodeQueue.push_front ( cursor );
            }
        }
        
        // ADVANCE chain (if blocks available)
        while ( this->mNodeQueue.size ()) {
        
            BlockTreeNode::ConstPtr next = this->mNodeQueue.front ();
            if ( !next->isComplete ()) break;
            
            this->pushBlock ( next->getBlock ());
            this->mNodeQueue.pop_front ();
        }
        
        // SEARCH for next missing block
        if ( this->mNodeQueue.size ()) {
            BlockTreeNode::ConstPtr search = this->mNodeQueue.front ();
            if ( search != this->mSearchTarget ) {
                this->mSearchTarget = search;
                this->mSearchCount = 0;
            }
        }
        
        // EXTEND once chain is complete and has consensus
        if ( this->hasConsensus ()) {
            this->extend ();
        }

        // SCAN the ledger for miners
        this->discoverMiners ();
        
        // QUERY the network for blocks
        this->startTasks ();
    }
}

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr Miner::trimExpired ( BlockTreeNode::ConstPtr tail ) {

    while ( tail && tail->isExpired ()) {
        tail = tail->getParent ();
    }
    return tail;
}

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr Miner::truncate ( BlockTreeNode::ConstPtr tail ) {

    if ( TheContext::get ().getRewriteMode () == TheContext::REWRITE_NONE ) return tail;

    // if a block from self would be more charming at any point along the chain,
    // truncate the chain to the parent of that block. in other words: seek the
    // earliest insertion point for a local block. if we find a block from
    // self, abort: to truncate, our local block must *beat* any other block.

    // TODO: this should take into account the lookback window.

    BlockTreeNode::ConstPtr cursor = tail;
    
    while ( cursor ) {
    
        const BlockHeader& header = **cursor;
    
        if (( TheContext::get ().getRewriteMode () == TheContext::REWRITE_WINDOW ) && !header.isInRewriteWindow ()) return tail;
    
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

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Miner::AbstractMiningMessengerClient_receive ( const MiningMessengerRequest& request, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block ) {
    
    unique_ptr < BlockQueueEntry > blockQueueEntry = make_unique < BlockQueueEntry >();
    blockQueueEntry->mRequest           = request;
    blockQueueEntry->mBlockHeader       = header;
    blockQueueEntry->mBlock             = block;

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
time_t Miner::Miner_getTime () const {

    time_t now;
    time ( &now );
    return now;
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
