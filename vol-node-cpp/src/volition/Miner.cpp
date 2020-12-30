// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Digest.h>
#include <volition/FileSys.h>
#include <volition/HTTPMiningMessenger.h>
#include <volition/Miner.h>
#include <volition/MinerLaunchTests.h>
#include <volition/Transaction.h>
#include <volition/Transactions.h>
#include <volition/UnsecureRandom.h>

namespace Volition {

//================================================================//
// BlockSearch
//================================================================//
    
//----------------------------------------------------------------//
void BlockSearch::step ( Miner& miner ) {

    if ( this->mCurrentSearch ) return; // search already in progress

    if ( this->mSearchTarget->checkStatus (( BlockTreeNode::Status )( BlockTreeNode::STATUS_NEW | BlockTreeNode::STATUS_MISSING ))) {

        set < shared_ptr < RemoteMiner >>::iterator remoteMinerIt = miner.mOnlineMiners.begin ();
        for ( ; remoteMinerIt != miner.mOnlineMiners.end (); ++remoteMinerIt ) {
        
            shared_ptr < RemoteMiner > remoteMiner = *remoteMinerIt;
            
            if ( this->mCompletedMiners.find ( remoteMiner->mMinerID ) == this->mCompletedMiners.end ()) {
                
                this->mCompletedMiners.insert ( remoteMiner->mMinerID );
                this->mCurrentSearch = remoteMiner;
                
                miner.mMessenger->enqueueBlockRequest ( remoteMiner->mURL, ( **this->mSearchTarget ).getDigest ());
                return;
            }
        }
        miner.mBlockTree.mark ( this->mSearchTarget, BlockTreeNode::STATUS_MISSING );
    }
}

//----------------------------------------------------------------//
void BlockSearch::step ( shared_ptr < RemoteMiner > remoteMiner ) {

    assert ( this->mCurrentSearch == remoteMiner );
    this->mCurrentSearch = NULL;
}

//================================================================//
// Miner
//================================================================//

//----------------------------------------------------------------//
void Miner::affirmBlockSearch ( BlockTreeNode::ConstPtr node ) {

    if ( node->getBlock ()) return;

    string hash = ( **node ).getDigest ();
    if ( this->mBlockSearches.find ( hash ) != this->mBlockSearches.end ()) return; // already searching

    BlockSearch& search = this->mBlockSearches [ hash ];
    search.mSearchTarget = node;
}

//----------------------------------------------------------------//
void Miner::affirmBranchSearch ( BlockTreeNode::ConstPtr node ) {

    while ( node && ( node->getBlock () == NULL )) {
        if (( **node ).getMinerID () != this->mMinerID ) {
            this->affirmBlockSearch ( node );
        }
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
void Miner::affirmRemoteMiner ( string url ) {

    if ( url.size () && ( this->mRemoteMinersByURL.find ( url ) == this->mRemoteMinersByURL.cend ())) {
        this->mNewMinerURLs.insert ( url );
    }
}

//----------------------------------------------------------------//
void Miner::affirmVisage () {

    assert ( this->mKeyPair );
    this->mVisage = this->mKeyPair.sign ( this->mMotto, Digest::HASH_ALGORITHM_SHA256 );
}

//----------------------------------------------------------------//
bool Miner::checkBestBranch ( string miners ) const {

    assert ( this->mWorkingLedger );
    return this->mWorkingLedger->checkMiners ( miners );
}

//----------------------------------------------------------------//
double Miner::checkConsensus ( BlockTreeNode::ConstPtr tag ) const {

    double count = 1;
    double current = 1;

    set < shared_ptr < RemoteMiner >>::const_iterator minerIt = this->mOnlineMiners.cbegin ();
    for ( ; minerIt != this->mOnlineMiners.cend (); ++minerIt ) {
        shared_ptr < RemoteMiner > remoteMiner = *minerIt;
        if ( !remoteMiner->mImproved ) continue;
        if ( tag->isAncestorOf ( remoteMiner->mImproved )) count += 1;
        current += 1;
    }
    return ( count / current );
}

//----------------------------------------------------------------//
void Miner::composeChain () {

    // TODO: gather transactions if rewinding chain

    if ( this->mWorkingLedgerTag == this->mBestProvisional ) return;

    // check to see if chain tag is *behind* best branch
    if ( this->mBestProvisional->isAncestorOf ( this->mWorkingLedgerTag )) {
        this->mWorkingLedger->reset (( **this->mBestProvisional ).getHeight () + 1 );
        this->mWorkingLedgerTag = this->mBestProvisional;
        return;
    }

    // if chain is divergent from best branch, re-root it
    if ( !this->mWorkingLedgerTag->isAncestorOf ( this->mBestProvisional )) {
        
        // REWIND chain to point of divergence
        BlockTreeNode::ConstPtr root = BlockTreeNode::findRoot ( this->mWorkingLedgerTag, this->mBestProvisional ).mRoot;
        assert ( root ); // guaranteed -> common genesis
        assert ( root->checkStatus ( BlockTreeNode::STATUS_COMPLETE ));  // guaranteed -> was in chain
        
        this->mWorkingLedger->reset (( **root ).getHeight () + 1 );
        this->mWorkingLedgerTag = root;
    }
    assert ( this->mWorkingLedgerTag->isAncestorOf ( this->mBestProvisional ));
    
    this->composeChainRecurse ( this->mBestProvisional );
}

//----------------------------------------------------------------//
void Miner::composeChainRecurse ( BlockTreeNode::ConstPtr branch ) {

    if ( this->mWorkingLedgerTag == branch ) return; // nothing to do
    
    BlockTreeNode::ConstPtr parent = branch->getParent ();
    if ( parent != this->mWorkingLedgerTag ) {
        this->composeChainRecurse ( parent );
    }
    
    if ( branch->isComplete ()) {
        this->pushBlock ( branch->getBlock ()); // TODO: handle invalid blocks
        assert ( this->mWorkingLedgerTag == branch );
    }
}

//----------------------------------------------------------------//
void Miner::discoverMiners () {

    // get miner list from ledger
    set < string > miners = this->getWorkingLedger ().getMiners ();
    set < string >::iterator minerIt = miners.begin ();
    for ( ; minerIt != miners.end (); ++minerIt ) {
        
        string minerID = *minerIt;
        
        if ( minerID != this->mMinerID ) {
                        
            if ( this->mRemoteMinersByID.find ( minerID ) == this->mRemoteMinersByID.cend ()) {
                AccountODBM minerODBM ( this->getWorkingLedger (), *minerIt );
                string url = minerODBM.mMinerInfo.get ()->getURL ();
                this->mNewMinerURLs.insert ( url );
            }
            else {
                shared_ptr < RemoteMiner > remoteMiner = this->mRemoteMinersByID [ minerID ];
                if ( remoteMiner && ( remoteMiner->mNetworkState != RemoteMiner::STATE_ONLINE )) {
                    this->mNewMinerURLs.insert ( remoteMiner->mURL );
                }
            }
        }
    }
    
    while ( this->mNewMinerURLs.size ()) {
        string url = *this->mNewMinerURLs.begin ();
        if ( url != this->mURL ) {
            this->mMessenger->enqueueMinerInfoRequest ( *this->mNewMinerURLs.begin ());
        }
        this->mNewMinerURLs.erase ( this->mNewMinerURLs.begin ());
    }
    
    // poll network for miner URLs
    if ( !this->mNetworkSearch ) {
        set < string > urls = this->sampleActiveMinerURLs ( 1 );
        if ( urls.size ()) {
            this->mMessenger->enqueueExtendNetworkRequest ( *urls.cbegin ());
            this->mNetworkSearch = true;
        }
    }
}

//----------------------------------------------------------------//
void Miner::extend ( time_t now ) {
    
    BlockTreeNode::ConstPtr provisional = this->mBestProvisional;
    if ( !provisional || provisional->isComplete ()) return;
    if (( **provisional ).getMinerID () != this->mMinerID ) return;
    
    BlockTreeNode::ConstPtr parent = provisional->getParent ();
    if ( !( parent && parent->isComplete ())) return;
    if ( this->checkConsensus ( parent ) < 0.5 ) return;

    assert ( this->mWorkingLedgerTag == parent );
    assert ( this->mPermanentLedgerTag != provisional );
    assert ( this->mPermanentLedgerTag->isAncestorOf ( provisional ));

    shared_ptr < Block > block = this->prepareBlock ( now );
    if ( block ) {
        
        assert ( block->getCharm () == ( **provisional ).getCharm ());
        
        this->mBestProvisional = parent;
        this->pushBlock ( block );
        this->scheduleReport ();
    }
}

//----------------------------------------------------------------//
BlockSearch* Miner::findBlockSearch ( const Digest& digest ) {

    map < string, BlockSearch >::iterator searchIt = this->mBlockSearches.find ( digest.toHex ());
    if ( searchIt == this->mBlockSearches.cend ()) return NULL; // no search; bail.
    
    return &searchIt->second;
}

//----------------------------------------------------------------//
const set < string >& Miner::getActiveMinerURLs () const {

    return this->mActiveMinerURLs;
}

//----------------------------------------------------------------//
size_t Miner::getChainSize () const {

    return this->mWorkingLedgerTag ? (( **this->mWorkingLedgerTag ).getHeight () + 1 ) : 0;
}

//----------------------------------------------------------------//
Ledger& Miner::getHighConfidenceLedger () {

    return this->mPermanentLedger;
}

//----------------------------------------------------------------//
TransactionStatus Miner::getTransactionStatus ( string accountName, string uuid ) const {

    if ( this->isBlocked ( accountName )) {
        return this->getLastStatus ( accountName );
    }

    const Ledger& ledger = this->getHighConfidenceLedger ();

    if ( ledger.hasTransaction ( accountName, uuid )) {
        return TransactionStatus ( TransactionStatus::ACCEPTED, "Transaction was accepted and applied.", uuid );
    }

    if ( this->hasTransaction ( accountName, uuid )) {
        return TransactionStatus ( TransactionStatus::PENDING, "Transaction is pending in queue.", uuid );
    }
    
    return TransactionStatus ( TransactionStatus::UNKNOWN, "Transaction is unknown.", uuid );
}

//----------------------------------------------------------------//
Ledger& Miner::getWorkingLedger () {

    assert ( this->mWorkingLedger );
    return *this->mWorkingLedger;
}

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr Miner::improveBranch ( BlockTreeNode::ConstPtr tail, time_t now ) {

    // if muted, no change is possible
    if ( this->mFlags & MINER_MUTE ) return tail;

    // first, check to see if we're authorized to mine. if we're not, then we can't change the branch, so return.
    AccountODBM accountODBM ( *this->mWorkingLedger, this->mMinerID );
    if ( !accountODBM.isMiner ()) return tail;

    // we *are* authorized to mine, so find the height common root of the incoming branch and the working ledger branch. if the
    // root is earlier than the height at which we became a miner, then we can't know if we're a miner in the incoming branch.
    // rather than risk mining an invalid block, we'll leave the branch unaltered.
    
    BlockTreeRoot root = BlockTreeNode::findRoot ( this->mWorkingLedgerTag, tail );
    if (( **root.mRoot ).getHeight () < accountODBM.mMinerHeight.get ()) return tail;
    
    // at this stage, we're allowed to mine and we know that we're authorized on this branch. so see if we can improve
    // the branch. we can only improve it if we find a place were we can legally append (or replace) a block with a
    // more charming block than what's there. we'll need to consider the rewrite window (i.e. don't replace blocks older
    // than the rewrite limit). we'll also need to consider the block delay (i.e. don't append a block before its time).
    // if we can't find an improvement, return the branch as-is. but if we *can* find an improvement, go ahead and
    // append (or replace) with a "provisional" node (i.e. a node with a header but no block).
    
    // there is also a special case when appending after one of our own blocks: the block node must be complete (i.e. not
    // be provisional).

    BlockTreeNode::ConstPtr parent = tail;
    BlockTreeNode::ConstPtr child;
    BlockTreeNode::ConstPtr extendFrom; // this is the *parent* of the block to append (if any).
    
    while ( parent ) {
    
        const BlockHeader& parentHeader = **parent;
        
        // if parent is one of ours, but it isn't yet complete, stop the search.
        if (( parentHeader.getMinerID () == this->mMinerID ) && ( !parent->checkStatus ( BlockTreeNode::STATUS_COMPLETE ))) break;
        
        // if enough time has elapsed since the parent was declared, we can consider replacing the child
        // with our own block (or appending a new block).
        if ( parentHeader.getNextTime () <= now ) {
            
            // check to see if our block would be more charming. if so, extend from the parent.
            if ( !child || BlockHeader::compare ( parentHeader.getNextCharm ( this->mVisage ), ( **child ).getCharm ()) < 0 ) {
                extendFrom = parent;
            }
        }
        
        // we can only replace blocks within the rewrite window. if parent is outside of that, no point in continuing.
        if (( this->mRewriteMode == BlockTreeNode::REWRITE_WINDOW ) && !parentHeader.isInRewriteWindow ( now )) break;
        
        // don't replace our own block; that would be silly.
        if ( parentHeader.getMinerID () == this->mMinerID ) break;
        
        child = parent;
        parent = parent->getParent ();
    }

    if ( extendFrom ) {
        return this->mBlockTree.affirmProvisional ( this->prepareProvisional ( **extendFrom, now ));
    }
    return tail; // use the chain as-is.
}

//----------------------------------------------------------------//
bool Miner::isLazy () const {

    return ( this->mActiveMinerURLs.size () == 0 );
}

//----------------------------------------------------------------//
shared_ptr < Block > Miner::loadGenesisBlock ( string path ) {

    shared_ptr < Block > block = make_shared < Block >();
    FromJSONSerializer::fromJSONFile ( *block, path );
    return block;
}

//----------------------------------------------------------------//
shared_ptr < Block > Miner::loadGenesisLedger ( string path ) {

    shared_ptr < Transactions::LoadLedger > loadLedger = make_shared < Transactions::LoadLedger >();
    FromJSONSerializer::fromJSONFile ( *loadLedger, path );
    
    shared_ptr < Transaction > transaction = make_shared < Transaction >();
    transaction->setBody ( loadLedger );
    
    shared_ptr < Block > block = make_shared < Block >();
    block->pushTransaction ( transaction );
    return block;
}

//----------------------------------------------------------------//
void Miner::loadKey ( string keyfile, string password ) {
    UNUSED ( password );

    // TODO: password

    this->mKeyPair.load ( keyfile );
    assert ( this->mKeyPair );
}

//----------------------------------------------------------------//
Miner::Miner () :
    mFlags ( DEFAULT_FLAGS ),
    mNeedsReport ( true ),
    mReportMode ( REPORT_NONE ),
    mRewriteMode ( BlockTreeNode::REWRITE_NONE ),
    mBlockVerificationPolicy ( Block::VerificationPolicy::ALL ),
    mControlLevel ( CONTROL_NONE ) {
    
    MinerLaunchTests::checkEnvironment ();
}

//----------------------------------------------------------------//
Miner::~Miner () {
}

//----------------------------------------------------------------//
void Miner::persist ( string path, shared_ptr < const Block > block ) {
    
    assert ( block );
    assert ( this->mWorkingLedger == NULL );
    
    Poco::Path basePath ( path );
    basePath.makeAbsolute ();
    path = basePath.toString ();
    mkdir ( path.c_str (), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

    string hash = block->getDigest ().toHex ();
    this->mLedgerFilename = Format::write ( "%s/%s.db", path.c_str (), hash.c_str ());
    this->mConfigFilename = Format::write ( "%s/%s-config.json", path.c_str (), hash.c_str ());
    
    this->mPersistenceProvider = make_shared < SQLiteStringStore >( this->mLedgerFilename );
    
    shared_ptr < Ledger > ledger = make_shared < Ledger >();
    ledger->takeSnapshot ( this->mPersistenceProvider, "master" );
        
    shared_ptr < const Block > topBlock = ledger->getBlock ();
    
    if ( topBlock ) {

        this->mWorkingLedger = ledger;

        this->mWorkingLedgerTag         = this->mBlockTree.affirmBlock ( topBlock );
        this->mPermanentLedger          = *this->mWorkingLedger;
        this->mPermanentLedgerTag       = this->mWorkingLedgerTag;
        this->mBestProvisional          = this->mWorkingLedgerTag;
    }
    
    this->setGenesis ( block );
    
    if ( FileSys::exists ( this->mConfigFilename )) {
        FromJSONSerializer::fromJSONFile ( this->mConfig, this->mConfigFilename );
    }
}

//----------------------------------------------------------------//
shared_ptr < Block > Miner::prepareBlock ( time_t now ) {
        
    shared_ptr < const Block > prevBlock = this->mWorkingLedger->getBlock ();
    assert ( prevBlock );
    
    shared_ptr < Block > block = make_shared < Block >(
        this->mMinerID,
        this->mVisage,
        now,
        prevBlock.get (),
        this->mKeyPair
    );
    
    block->setBlockDelayInSeconds( this->mWorkingLedger->getBlockDelayInSeconds ());
    block->setRewriteWindow ( this->mWorkingLedger->getRewriteWindowInSeconds ());
    
    this->fillBlock ( *this->mWorkingLedger, *block, this->mBlockVerificationPolicy, this->getMinimumGratuity ());
    
    if ( !( this->isLazy () && ( block->countTransactions () == 0 ))) {
        block->sign ( this->mKeyPair, Digest::DEFAULT_HASH_ALGORITHM );
        return block;
    }
    return NULL;
}

//----------------------------------------------------------------//
shared_ptr < BlockHeader > Miner::prepareProvisional ( const BlockHeader& parent, time_t now ) const {
    
    shared_ptr < BlockHeader > provisional = make_shared < BlockHeader >(
        this->mMinerID,
        this->mVisage,
        now,
        &parent,
        this->mKeyPair
    );
    
    provisional->setBlockDelayInSeconds ( this->mWorkingLedger->getBlockDelayInSeconds ());
    provisional->setRewriteWindow ( this->mWorkingLedger->getRewriteWindowInSeconds ());
    provisional->setDigest ( Digest ( *provisional ));
    
    return provisional;
}

//----------------------------------------------------------------//
void Miner::pruneTransactions () {

    this->TransactionQueue::pruneTransactions ( this->mPermanentLedger );
}

//----------------------------------------------------------------//
void Miner::pushBlock ( shared_ptr < const Block > block ) {

    bool result = this->mWorkingLedger->pushBlock ( *block, this->mBlockVerificationPolicy );
    assert ( result );
    
    BlockTreeNode::ConstPtr node = this->mBlockTree.affirmBlock ( block );
    assert ( node );
    
    if ( this->mWorkingLedgerTag == this->mBestProvisional ) {
        this->mBestProvisional = node;
    }
    this->mWorkingLedgerTag = node;
}

//----------------------------------------------------------------//
void Miner::report () const {

    switch ( this->mReportMode ) {
    
        case REPORT_BEST_BRANCH: {
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "%d: %s", ( int )( **this->mWorkingLedgerTag ).getHeight (), this->mWorkingLedgerTag->writeBranch ().c_str ());
            break;
        }
        
        case REPORT_ALL_BRANCHES: {
        
            map < string, shared_ptr < RemoteMiner >>::const_iterator remoteMinerIt = this->mRemoteMinersByURL.begin ();
            for ( ; remoteMinerIt != this->mRemoteMinersByURL.end (); ++remoteMinerIt ) {
            
                const RemoteMiner& remoteMiner = *remoteMinerIt->second;
                if ( remoteMiner.mTag ) {
                    LGN_LOG ( VOL_FILTER_ROOT, INFO,
                        "%s - %d: %s",
                        remoteMiner.mMinerID.c_str (),
                        ( int )( **remoteMiner.mTag ).getHeight (),
                        remoteMiner.mTag->writeBranch ().c_str ()
                    );
                }
                else {
                    LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s: MISSING TAG", remoteMiner.mMinerID.c_str ());
                }
            }
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "BEST - %d: %s", ( int )( **this->mWorkingLedgerTag ).getHeight (), this->mWorkingLedgerTag->writeBranch ().c_str ());
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "" );
            break;
        }
        
        case REPORT_NONE:
        default:
            break;
    }
}

//----------------------------------------------------------------//
void Miner::reset () {

    this->TransactionQueue::reset ();
    this->mWorkingLedger->reset ( 1 );
    this->mWorkingLedger->clearSchemaCache ();
    this->Miner_reset ();
}

//----------------------------------------------------------------//
set < string > Miner::sampleActiveMinerURLs ( size_t sampleSize ) const {

    set < string > miners = this->mActiveMinerURLs;
        
    if ( sampleSize < miners.size ()) {
        set < string > subset;
        for ( size_t i = 0; i < sampleSize; ++i ) {
            set < string >::iterator minerIt = miners.begin ();
            advance ( minerIt, ( long )( UnsecureRandom::get ().random ( 0, miners.size () - 1 ))); // this doesn't need to be cryptographically random; keep it simple
            subset.insert ( *minerIt );
            miners.erase ( minerIt );
        }
        miners = subset;
    }
    return miners;
}

//----------------------------------------------------------------//
void Miner::saveChain () {

    if ( this->mPersistenceProvider ) {
        this->mPermanentLedger.persist ( this->mPersistenceProvider, "master" );
    }
}

//----------------------------------------------------------------//
void Miner::saveConfig () {

    if ( this->mConfigFilename.size () > 0 ) {
        ToJSONSerializer::toJSONFile ( this->mConfig, this->mConfigFilename );
    }
}

//----------------------------------------------------------------//
void Miner::scheduleReport () {

    this->mNeedsReport = true;
}

//----------------------------------------------------------------//
void Miner::setGenesis ( shared_ptr < const Block > block ) {

    assert ( block );

    if ( this->mWorkingLedger ) {
    
        assert ( this->mWorkingLedgerTag );
        assert ( this->mWorkingLedger->countBlocks ());
        assert ( this->mWorkingLedger->getGenesisHash () == block->getDigest ().toHex ());
        assert ( this->mPermanentLedgerTag );
    }
    else {
    
        this->mWorkingLedger        = make_shared < Ledger >();
        this->mWorkingLedgerTag     = NULL;
        this->mBestProvisional      = NULL;
        this->mPermanentLedgerTag   = NULL;
        
        this->pushBlock ( block );
        
        this->updatePermanentTag ();
    }
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
void Miner::setRewriteWindow () {

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

    this->affirmMessenger ();
    this->mMessenger->await ();
    this->mMessenger->receiveResponses ( *this, now );
    
    // this applies any new headers and updates connectivity status
    this->updateRemoteMiners ();
    
    // this evaluates each branch and picks the best candidate
    this->updateBestBranch ( now );
    
    // using the best branch, build or rebuild chain
    this->composeChain ();
    
    // fill the provisional block (if any)
    this->extend ( now );
    
    this->updatePermanentTag ();
    this->pruneTransactions ();
    
    this->updateBlockSearches ();
    this->updateHeaderSearches ();
    this->discoverMiners ();
    this->mMessenger->sendRequests ();
    this->report ();
}

//----------------------------------------------------------------//
void Miner::updateBestBranch ( time_t now ) {

    // update the current best branch, excluding missing or invalid blocks.
    // this may append an additional provisional header if branch is complete.
    BlockTreeNode::ConstPtr bestBranch = this->improveBranch ( this->mBestProvisional->trimMissingOrInvalid (), now );

    set < shared_ptr < RemoteMiner >>::iterator remoteMinerIt = this->mOnlineMiners.begin ();
    for ( ; remoteMinerIt != this->mOnlineMiners.end (); ++remoteMinerIt ) {
        
        shared_ptr < RemoteMiner > remoteMiner = *remoteMinerIt;
        if ( !remoteMiner->mTag ) continue;
        
        remoteMiner->mImproved = this->improveBranch ( remoteMiner->mTag->trimInvalid (), now );
        if ( remoteMiner->mImproved->isMissing ()) continue;
        
        assert ( !remoteMiner->mImproved->isMissingOrInvalid ());
        
        if ( BlockTreeNode::compare ( remoteMiner->mImproved, bestBranch, this->mRewriteMode ) < 0 ) {
            bestBranch = remoteMiner->mImproved;
        }
    }
    
    assert ( bestBranch );
    assert ( !bestBranch->isMissingOrInvalid ());
    this->mBestProvisional = bestBranch;
}

//----------------------------------------------------------------//
void Miner::updateBlockSearches () {

    set < shared_ptr < RemoteMiner >>::const_iterator remoteMinerIt = this->mOnlineMiners.begin ();
    for ( ; remoteMinerIt != this->mOnlineMiners.end (); ++remoteMinerIt ) {
        
        shared_ptr < const RemoteMiner > remoteMiner = *remoteMinerIt;

        // we only care about missing branches; ignore new/complete/invalid branches.
        if ( remoteMiner->mImproved && remoteMiner->mImproved->isMissing ()) {
            
            // only affirm a search if the other chain could beat our current.
            if ( BlockTreeNode::compare ( remoteMiner->mImproved, this->mBestProvisional, this->mRewriteMode ) < 0 ) {
                this->affirmBranchSearch ( remoteMiner->mImproved );
            }
        }
    }
    
    // always affirm a search for the current branch
    this->affirmBranchSearch ( this->mBestProvisional );
    
    // step the currently active block searches
    map < string, BlockSearch >::iterator blockSearchIt = this->mBlockSearches.begin ();
    while ( blockSearchIt != this->mBlockSearches.end ()) {
    
        map < string, BlockSearch >::iterator cursor = blockSearchIt++;
    
        BlockSearch& blockSearch = cursor->second;
        blockSearch.step ( *this );
        if ( !blockSearch.mCurrentSearch ) {
            this->mBlockSearches.erase ( cursor );
        }
    }
}

//----------------------------------------------------------------//
void Miner::updateHeaderSearches () {
    
    set < shared_ptr < RemoteMiner >>::const_iterator remoteMinerIt = this->mOnlineMiners.cbegin ();
    for ( ; remoteMinerIt != this->mOnlineMiners.cend (); ++remoteMinerIt ) {
    
        shared_ptr < const RemoteMiner > remoteMiner = *remoteMinerIt;
        string url = remoteMiner->mURL;
        
        // constantly refill active set
        if ( this->mHeaderSearches.find ( url ) == this->mHeaderSearches.end ()) {
            this->mMessenger->enqueueHeaderRequest ( url, remoteMiner->mHeight, remoteMiner->mForward );
            this->mHeaderSearches.insert ( url );
        }
    }
}

//----------------------------------------------------------------//
void Miner::updatePermanentTag () {

    assert ( this->mWorkingLedger );

    // start with the current best provisional branch and walk back to a point where general consensus (among responding
    // miners) is greater than quorum (right now hardcoded at 60%) and/or where we hit the root (or the current permanent tag).
    BlockTreeNode::ConstPtr tag = this->mBestProvisional;
    while ( tag->getParent () && (( this->checkConsensus ( tag ) < 0.6 ) || ( !tag->getBlock ()))) tag = tag->getParent ();
    
    assert ( tag );
    assert ( tag->getBlock ());
    assert ( tag->isAncestorOf ( this->mBestProvisional ));
    
    if ( this->mPermanentLedgerTag != tag ) {
    
        this->mPermanentLedgerTag = tag;
        this->mPermanentLedger = *this->mWorkingLedger;
        this->mPermanentLedger.revert (( **this->mPermanentLedgerTag ).getHeight ());
        
        this->mBlockTree.setRoot ( this->mPermanentLedgerTag );
        
        this->saveChain ();
        
        set < shared_ptr < RemoteMiner >>::iterator remoteMinerIt = this->mOnlineMiners.begin ();
        for ( ; remoteMinerIt != this->mOnlineMiners.end (); ++remoteMinerIt ) {
        
            shared_ptr < RemoteMiner > remoteMiner = *remoteMinerIt;
            
            if ( remoteMiner->mTag && remoteMiner->mTag->isRefused ()) {
                remoteMiner->reset ();
            }
        }
    }
}

//----------------------------------------------------------------//
void Miner::updateRemoteMiners () {

    this->mOnlineMiners.clear ();
    this->mActiveMinerURLs.clear ();
    
    map < string, shared_ptr < RemoteMiner >>::iterator remoteMinerIt = this->mRemoteMinersByURL.begin ();
    for ( ; remoteMinerIt != this->mRemoteMinersByURL.end (); ++remoteMinerIt ) {
    
        shared_ptr < RemoteMiner > remoteMiner = remoteMinerIt->second;
        if ( remoteMiner->mNetworkState != RemoteMiner::STATE_ONLINE ) continue;
        
        this->mOnlineMiners.insert ( remoteMiner );;
        
        remoteMiner->updateHeaders ( this->mBlockTree );
        
        if ( remoteMiner->mTag ) {
            this->mActiveMinerURLs.insert ( remoteMiner->mURL );
        }
    }
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Miner::AbstractMiningMessengerClient_receiveResponse ( const MiningMessengerResponse& response, time_t now ) {

    this->scheduleReport ();

    const MiningMessengerRequest& request = response.mRequest;
    string url = response.mRequest.mMinerURL;
    MiningMessengerResponse::Status status = response.mStatus;
    
    // TODO: these could be set deliberately as an attack; fix by sending up a nonce
    if ( url == this->mURL ) return;
    if ( response.mMinerID == this->mMinerID ) return;
    
    map < string, shared_ptr < RemoteMiner >>::iterator remoteMinerIt = this->mRemoteMinersByURL.find ( url );
    shared_ptr < RemoteMiner > remoteMiner = remoteMinerIt != this->mRemoteMinersByURL.cend () ? remoteMinerIt->second : NULL;
    
    switch ( request.mRequestType ) {
        
        case MiningMessengerRequest::REQUEST_BLOCK: {
            
            assert ( remoteMiner );
            this->mBlockTree.update ( response.mBlock );
            assert ( this->mBlockSearches.find ( request.mBlockDigest.toHex ()) != this->mBlockSearches.end ());
            this->mBlockSearches [ request.mBlockDigest.toHex ()].step ( remoteMiner );
            break;
        }
        
        case MiningMessengerRequest::REQUEST_EXTEND_NETWORK: {
            
            set < string >::const_iterator urlIt = response.mMinerURLs.cbegin ();
            for ( ; urlIt != response.mMinerURLs.cend (); ++urlIt ) {
                this->affirmRemoteMiner ( *urlIt );
            }
            this->mNetworkSearch = false;
            break;
        }
        
        case MiningMessengerRequest::REQUEST_HEADERS: {
                            
            assert ( remoteMiner );
            
            list < shared_ptr < const BlockHeader >>::const_iterator headerIt = response.mHeaders.cbegin ();
            for ( ; headerIt != response.mHeaders.cend (); ++headerIt ) {
                
                shared_ptr < const BlockHeader > header = *headerIt;
                if ( !header ) continue;
                if ( header->getTime () > now ) continue; // ignore headers from the future
                
                if ( header->getHeight () == 0 ) {
                    BlockTreeNode::ConstPtr root = this->mBlockTree.getRoot ();
                    if (( **root ).getDigest () != header->getDigest ()) {
                        remoteMiner->setError ( "Unrecoverable error: genesis block mismatch." );
                        break;
                    }
                }
                remoteMiner->mHeaderQueue [ header->getHeight ()] = header;
            }
            
            this->mHeaderSearches.erase ( response.mRequest.mMinerURL );
            break;
        }
        
        case MiningMessengerRequest::REQUEST_MINER_INFO: {

            if ( status == MiningMessengerResponse::STATUS_OK ) {

                if ( !remoteMiner ) {
                    
                    remoteMiner                 = make_shared < RemoteMiner >();
                    remoteMiner->mURL           = url;
                    remoteMiner->mMinerID       = response.mMinerID;
                    
                    this->mRemoteMinersByURL [ url ]                    = remoteMiner;
                    this->mRemoteMinersByID [ remoteMiner->mMinerID ]   = remoteMiner;
                }

                if ( remoteMiner->mMinerID != response.mMinerID ) {
                    this->mRemoteMinersByID.erase ( remoteMiner->mMinerID );
                    this->mRemoteMinersByID [ remoteMiner->mMinerID ] = remoteMiner;
                }
            }
            break;
        }
        
        default:
            assert ( false );
            break;
    }
    
    if ( remoteMiner ) {
        if ( status == MiningMessengerResponse::STATUS_OK ) {
            remoteMiner->mNetworkState = RemoteMiner::STATE_ONLINE;
        }
        else {
            remoteMiner->setError ();
        }
    }
}

//----------------------------------------------------------------//
void Miner::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    UNUSED ( serializer );
}

//----------------------------------------------------------------//
void Miner::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
    UNUSED ( serializer );
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
