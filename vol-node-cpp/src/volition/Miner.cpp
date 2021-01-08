// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Digest.h>
#include <volition/FileSys.h>
#include <volition/HTTPMiningMessenger.h>
#include <volition/InMemoryBlockTree.h>
#include <volition/Miner.h>
#include <volition/MinerLaunchTests.h>
#include <volition/SQLiteBlockTree.h>
#include <volition/Transaction.h>
#include <volition/Transactions.h>
#include <volition/UnsecureRandom.h>

namespace Volition {

//================================================================//
// BlockSearch
//================================================================//

//----------------------------------------------------------------//
BlockSearch::BlockSearch () {
}

//----------------------------------------------------------------//
bool BlockSearch::step ( Miner& miner ) {

    if ( SEARCH_SIZE <= this->mActiveMiners.size ()) return true;

    BlockTreeCursor cursor = miner.mBlockTree->findCursorForHash ( this->mHash );
    if ( !( cursor.hasHeader () && cursor.checkStatus (( kBlockTreeEntryStatus )( kBlockTreeEntryStatus::STATUS_NEW | kBlockTreeEntryStatus::STATUS_MISSING )))) return false;

    set < shared_ptr < RemoteMiner >> remoteMiners;
    set < shared_ptr < RemoteMiner >>::iterator remoteMinerIt = miner.mOnlineMiners.begin ();
    for ( ; remoteMinerIt != miner.mOnlineMiners.end (); ++remoteMinerIt ) {
    
        shared_ptr < RemoteMiner > remoteMiner = *remoteMinerIt;
        string minerID = remoteMiner->getMinerID ();
        
        if ( this->mCompletedMiners.find ( minerID ) != this->mCompletedMiners.end ()) continue;
        if ( this->mActiveMiners.find ( minerID ) != this->mActiveMiners.end ()) continue;
        
        remoteMiners.insert ( remoteMiner );
    }
            
    size_t sampleSize = ( SEARCH_SIZE - this->mActiveMiners.size ());
    
    for ( size_t i = 0; (( i < sampleSize ) && ( remoteMiners.size () > 0 )); ++i ) {
    
        remoteMinerIt = remoteMiners.begin ();
        advance ( remoteMinerIt, ( long )( UnsecureRandom::get ().random ( 0, remoteMiners.size () - 1 )));
        remoteMiners.erase ( remoteMinerIt );
        
        shared_ptr < RemoteMiner > remoteMiner = *remoteMinerIt;
        this->mActiveMiners.insert ( remoteMiner->getMinerID ());
        miner.mMessenger->enqueueBlockRequest (
            remoteMiner->mURL,
            cursor.getDigest (),
            cursor.getHeight (),
            Format::write ( "%s:%s", miner.mMinerID.c_str (), cursor.getCharmTag ().c_str ())
        );
    }
    
    if ( this->mActiveMiners.size () == 0 ) {
        miner.mBlockTree->mark ( cursor, kBlockTreeEntryStatus::STATUS_MISSING );
        return false;
    }
    return true;
}

//----------------------------------------------------------------//
void BlockSearch::step ( shared_ptr < RemoteMiner > remoteMiner ) {

    string minerID = remoteMiner->getMinerID ();
    assert ( this->mActiveMiners.find ( minerID ) != this->mActiveMiners.cend ());
    this->mCompletedMiners.insert ( minerID );
    this->mActiveMiners.erase ( minerID );
}

//================================================================//
// Miner
//================================================================//

//----------------------------------------------------------------//
void Miner::affirmBlockSearch ( BlockTreeCursor cursor ) {

    if ( cursor.getBlock ()) return;

    string hash = cursor.getDigest ();
    if ( this->mBlockSearches.find ( hash ) != this->mBlockSearches.end ()) return; // already searching

    BlockSearch& search = this->mBlockSearches [ hash ];
    search.mHash = cursor.getHash ();
}

//----------------------------------------------------------------//
void Miner::affirmBranchSearch ( BlockTreeCursor cursor ) {

    while ( cursor.hasHeader () && ( cursor.getBlock () == NULL )) {
        if ( cursor.getMinerID () != this->mMinerID ) {
            this->affirmBlockSearch ( cursor );
        }
        cursor = cursor.getParent ();
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

    this->mVisage = Miner::calculateVisage ( this->mKeyPair, this->mMotto );
}

//----------------------------------------------------------------//
Signature Miner::calculateVisage ( const CryptoKeyPair& keyPair, string motto ) {

    assert ( keyPair );
    return keyPair.sign ( motto, Digest::HASH_ALGORITHM_SHA256 );
}

//----------------------------------------------------------------//
bool Miner::checkBestBranch ( string miners ) const {

    assert ( this->mLedger );
    return this->mLedger->checkMiners ( miners );
}

//----------------------------------------------------------------//
double Miner::checkConsensus ( BlockTreeCursor tag ) const {

    double count = 1;
    double current = 1;

    set < shared_ptr < RemoteMiner >>::const_iterator minerIt = this->mOnlineMiners.cbegin ();
    for ( ; minerIt != this->mOnlineMiners.cend (); ++minerIt ) {
        shared_ptr < RemoteMiner > remoteMiner = *minerIt;
        if ( remoteMiner->mImproved.hasCursor () && tag.isAncestorOf ( *remoteMiner->mImproved )) {
            count += 1;
        }
        current += 1;
    }
    return ( count / current );
}

//----------------------------------------------------------------//
void Miner::composeChain () {

    // TODO: gather transactions if rewinding chain

    if ( this->mLedgerTag.equals ( this->mBestBranchTag )) return;

    // check to see if chain tag is *behind* best branch
    if (( *this->mBestBranchTag ).isAncestorOf ( *this->mLedgerTag )) {
        this->mLedger->reset ( this->mBestBranchTag.getHeight () + 1 );
        this->mBlockTree->tag ( this->mLedgerTag, this->mBestBranchTag );
        return;
    }

    // if chain is divergent from best branch, re-root it
    if ( !( *this->mLedgerTag ).isAncestorOf ( *this->mBestBranchTag )) {
        
        // REWIND chain to point of divergence
        BlockTreeCursor root = BlockTreeCursor::findRoot ( *this->mLedgerTag, *this->mBestBranchTag );
        assert ( root.hasHeader ()); // guaranteed -> common genesis
        assert ( root.checkStatus ( kBlockTreeEntryStatus::STATUS_COMPLETE ));  // guaranteed -> was in chain
        
        this->mLedger->reset ( root.getHeight () + 1 );
        this->mBlockTree->tag ( this->mLedgerTag, root );
    }
    assert (( *this->mLedgerTag ).isAncestorOf ( *this->mBestBranchTag ));
    
    this->composeChainRecurse ( *this->mBestBranchTag );
}

//----------------------------------------------------------------//
void Miner::composeChainRecurse ( BlockTreeCursor branch ) {

    if (( *this->mLedgerTag ).equals ( branch )) return; // nothing to do
    
    BlockTreeCursor parent = branch.getParent ();
    if ( !parent.equals ( *this->mLedgerTag )) {
        this->composeChainRecurse ( parent );
    }
    
    if ( branch.isComplete ()) {
        this->pushBlock ( branch.getBlock ()); // TODO: handle invalid blocks
        assert (( *this->mLedgerTag ).equals ( branch ));
    }
}

//----------------------------------------------------------------//
void Miner::discoverMiners () {

    // get miner list from ledger
    set < string > miners = this->getLedger ().getMiners ();
    set < string >::iterator minerIt = miners.begin ();
    for ( ; minerIt != miners.end (); ++minerIt ) {
        
        string minerID = *minerIt;
        
        if ( minerID != this->mMinerID ) {
                        
            if ( this->mRemoteMinersByID.find ( minerID ) == this->mRemoteMinersByID.cend ()) {
                AccountODBM minerODBM ( this->getLedger (), *minerIt );
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
    
    BlockTreeCursor provisional = *this->mBestBranchTag;
    if ( !provisional.hasHeader () || provisional.isComplete ()) return;
    if ( provisional.getMinerID () != this->mMinerID ) return;
    
    BlockTreeCursor parent = provisional.getParent ();
    if ( !( parent.hasHeader () && parent.isComplete ())) return;
    if ( this->checkConsensus ( parent ) <= 0.5 ) return;

    assert (( *this->mLedgerTag ).equals ( parent ));

    shared_ptr < Block > block = this->prepareBlock ( now );
    if ( block ) {
        
        assert ( block->getCharm () == provisional.getCharm ());
        
//        this->mBestProvisional = parent;
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

    return this->mLedgerTag.hasCursor () ? ( this->mLedgerTag.getHeight () + 1 ) : 0;
}

//----------------------------------------------------------------//
Ledger& Miner::getLedger () {

    assert ( this->mLedger );
    return *this->mLedger;
}

//----------------------------------------------------------------//
TransactionStatus Miner::getTransactionStatus ( string accountName, string uuid ) const {

    if ( this->isBlocked ( accountName )) {
        return this->getLastStatus ( accountName );
    }

    const Ledger& ledger = this->getLedger ();

    if ( ledger.hasTransaction ( accountName, uuid )) {
        return TransactionStatus ( TransactionStatus::ACCEPTED, "Transaction was accepted and applied.", uuid );
    }

    if ( this->hasTransaction ( accountName, uuid )) {
        return TransactionStatus ( TransactionStatus::PENDING, "Transaction is pending in queue.", uuid );
    }
    
    return TransactionStatus ( TransactionStatus::UNKNOWN, "Transaction is unknown.", uuid );
}

//----------------------------------------------------------------//
BlockTreeCursor Miner::improveBranch ( BlockTreeTag& tag, BlockTreeCursor tail, time_t now ) {

    // if muted, no change is possible
    if ( this->mFlags & MINER_MUTE ) return tail;

    // first, check to see if we're authorized to mine. if we're not, then we can't change the branch, so return.
    AccountODBM accountODBM ( *this->mLedger, this->mMinerID );
    if ( !accountODBM.isMiner ()) return tail;

    // height at which we became a miner (according to current working ledger).
    u64 minerHeight = accountODBM.mMinerHeight.get ();

    // we *are* authorized to mine, so find the height of the common root of the incoming branch and the working ledger branch.
    // if then root is earlier than the height at which we became a miner, then we can't know if we're a miner in the incoming branch.
    // rather than risk mining an invalid block, we'll leave the branch unaltered.
    
    BlockTreeCursor root = BlockTreeCursor::findRoot ( *this->mLedgerTag, tail );
    if ( root.getHeight () < minerHeight ) return tail;
    
    // at this stage, we're allowed to mine and we know that we're authorized on this branch. so see if we can improve
    // the branch. we can only improve it if we find a place were we can legally append (or replace) a block with a
    // more charming block than what's there. we'll need to consider the rewrite window (i.e. don't replace blocks older
    // than the rewrite limit). we'll also need to consider the block delay (i.e. don't append a block before its time).
    // if we can't find an improvement, return the branch as-is. but if we *can* find an improvement, go ahead and
    // append (or replace) with a "provisional" node (i.e. a node with a header but no block).
    
    // there is also a special case when appending after one of our own blocks: the block node must be complete (i.e. not
    // be provisional).

    BlockTreeCursor parent = tail;
    BlockTreeCursor child;
    BlockTreeCursor extendFrom; // this is the *parent* of the block to append (if any).
    
    while ( parent.hasHeader ()) {
    
        const BlockHeader& parentHeader = parent.getHeader ();
        
        // if parent is one of ours, but it isn't yet complete, stop the search.
        if (( parentHeader.getMinerID () == this->mMinerID ) && ( !parent.checkStatus ( kBlockTreeEntryStatus::STATUS_COMPLETE ))) break;
        
        // if enough time has elapsed since the parent was declared, we can consider replacing the child
        // with our own block (or appending a new block).
        if ( parentHeader.getNextTime () <= now ) {
            
            // check to see if our block would be more charming. if so, extend from the parent.
            if ( !child.hasHeader () || BlockHeader::compare ( parentHeader.getNextCharm ( this->mVisage ), child.getCharm ()) < 0 ) {
                extendFrom = parent;
            }
        }
        
        // we can only replace blocks within the rewrite window. if parent is outside of that, no point in continuing.
        if (( this->mRewriteMode == kRewriteMode::REWRITE_WINDOW ) && !parentHeader.isInRewriteWindow ( now )) break;
        
        // don't replace our own block; that would be silly.
        if ( parentHeader.getMinerID () == this->mMinerID ) break;
        
        // don't replace the block in which we became a miner.
        if ( parent.getHeight () < minerHeight ) break;
        
        child = parent;
        parent = parent.getParent ();
    }

    if ( extendFrom.hasHeader ()) {
        return this->mBlockTree->affirmProvisional ( tag, this->prepareProvisional ( extendFrom.getHeader (), now ));
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
    mRewriteMode ( kRewriteMode::REWRITE_NONE ),
    mBlockVerificationPolicy ( Block::VerificationPolicy::ALL ),
    mControlLevel ( CONTROL_NONE ) {
    
    this->mLedgerTag.setName ( "working" );
    this->mBestBranchTag.setName ( "best" );
    
    MinerLaunchTests::checkEnvironment ();
    
    this->mBlockTree = make_shared < InMemoryBlockTree >();
}

//----------------------------------------------------------------//
Miner::~Miner () {
}

//----------------------------------------------------------------//
void Miner::persist ( string path, shared_ptr < const Block > block ) {
    
    assert ( block );
    assert ( this->mLedger == NULL );
    
    Poco::Path basePath ( path );
    basePath.makeAbsolute ();
    path = basePath.toString ();
    mkdir ( path.c_str (), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

    string hash = block->getDigest ().toHex ();
    this->mLedgerFilename = Format::write ( "%s/%s.db", path.c_str (), hash.c_str ());
    this->mConfigFilename = Format::write ( "%s/%s-config.json", path.c_str (), hash.c_str ());
    this->mBlocksFilename = Format::write ( "%s/%s-blocks.db", path.c_str (), hash.c_str ());
    
    shared_ptr < SQLiteBlockTree > blockTree = make_shared < SQLiteBlockTree >( this->mBlocksFilename );
    
    this->mPersistenceProvider = make_shared < SQLiteStringStore >( this->mLedgerFilename );
    
    shared_ptr < Ledger > ledger = make_shared < Ledger >();
    ledger->takeSnapshot ( this->mPersistenceProvider, "master" );
        
    shared_ptr < const Block > topBlock = ledger->getBlock ();
    
    if ( topBlock ) {
        this->mLedger = ledger;
        this->mBlockTree->affirmBlock ( this->mLedgerTag, topBlock );
        this->mBlockTree->tag ( this->mBestBranchTag, this->mLedgerTag );
    }
    
    this->setGenesis ( block );
    
    if ( FileSys::exists ( this->mConfigFilename )) {
        FromJSONSerializer::fromJSONFile ( this->mConfig, this->mConfigFilename );
    }
}

//----------------------------------------------------------------//
shared_ptr < Block > Miner::prepareBlock ( time_t now ) {
        
    shared_ptr < const Block > prevBlock = this->mLedger->getBlock ();
    assert ( prevBlock );
    
    shared_ptr < Block > block = make_shared < Block >();
    block->initialize (
        this->mMinerID,
        this->mVisage,
        now,
        prevBlock.get (),
        this->mKeyPair
    );
    
    block->setBlockDelayInSeconds( this->mLedger->getBlockDelayInSeconds ());
    block->setRewriteWindow ( this->mLedger->getRewriteWindowInSeconds ());
    
    this->fillBlock ( *this->mLedger, *block, this->mBlockVerificationPolicy, this->getMinimumGratuity ());
    
    if ( !( this->isLazy () && ( block->countTransactions () == 0 ))) {
        block->sign ( this->mKeyPair, Digest::DEFAULT_HASH_ALGORITHM );
        return block;
    }
    return NULL;
}

//----------------------------------------------------------------//
shared_ptr < BlockHeader > Miner::prepareProvisional ( const BlockHeader& parent, time_t now ) const {
    
    shared_ptr < BlockHeader > provisional = make_shared < BlockHeader >();
    provisional->initialize (
        this->mMinerID,
        this->mVisage,
        now,
        &parent,
        this->mKeyPair
    );
    
    provisional->setBlockDelayInSeconds ( this->mLedger->getBlockDelayInSeconds ());
    provisional->setRewriteWindow ( this->mLedger->getRewriteWindowInSeconds ());
    provisional->setDigest ( Digest ( *provisional ));
    
    return provisional;
}

//----------------------------------------------------------------//
void Miner::pruneTransactions () {

    this->TransactionQueue::pruneTransactions ( *this->mLedger );
}

//----------------------------------------------------------------//
void Miner::pushBlock ( shared_ptr < const Block > block ) {

    this->mLedger->revert ( block->getHeight ());

    bool result = this->mLedger->pushBlock ( *block, this->mBlockVerificationPolicy );
    assert ( result );
    
    BlockTreeCursor node = this->mBlockTree->affirmBlock ( this->mBestBranchTag, block );
    assert ( node.hasHeader ());
    
    this->mBlockTree->tag ( this->mLedgerTag, node );
}

//----------------------------------------------------------------//
void Miner::report () const {

    switch ( this->mReportMode ) {
    
        case REPORT_BEST_BRANCH: {
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "%d: %s", ( int )this->mLedgerTag.getHeight (), ( *this->mLedgerTag ).writeBranch ().c_str ());
            break;
        }
        
        case REPORT_ALL_BRANCHES: {
        
            map < string, shared_ptr < RemoteMiner >>::const_iterator remoteMinerIt = this->mRemoteMinersByURL.begin ();
            for ( ; remoteMinerIt != this->mRemoteMinersByURL.end (); ++remoteMinerIt ) {
            
                const RemoteMiner& remoteMiner = *remoteMinerIt->second;
                if ( remoteMiner.mTag.hasCursor ()) {
                    LGN_LOG ( VOL_FILTER_ROOT, INFO,
                        "%s - %d: %s",
                        remoteMiner.getMinerID ().c_str (),
                        ( int )remoteMiner.mTag.getHeight (),
                        ( *remoteMiner.mTag ).writeBranch ().c_str ()
                    );
                }
                else {
                    LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s: MISSING TAG", remoteMiner.getMinerID ().c_str ());
                }
            }
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "BEST - %d: %s", ( int )( *this->mLedgerTag ).getHeight (), ( *this->mLedgerTag ).writeBranch ().c_str ());
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
    this->mLedger->reset ( 1 );
    this->mLedger->clearSchemaCache ();
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

    if ( this->mLedger && this->mPersistenceProvider ) {
        this->mLedger->persist ( this->mPersistenceProvider, "master" );
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

    if ( this->mLedger ) {
    
        assert ( this->mLedgerTag.hasCursor ());
        assert ( this->mLedger->countBlocks ());
        assert ( this->mLedger->getGenesisHash () == block->getDigest ().toHex ());
    }
    else {
    
        this->mLedger = make_shared < Ledger >();
        this->pushBlock ( block );
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

    this->setRewriteMode ( kRewriteMode::REWRITE_WINDOW );
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
    
    this->improveBranch ( this->mBestBranchTag, ( *this->mBestBranchTag ).trimMissingOrInvalid (), now );

    set < shared_ptr < RemoteMiner >>::iterator remoteMinerIt = this->mOnlineMiners.begin ();
    for ( ; remoteMinerIt != this->mOnlineMiners.end (); ++remoteMinerIt ) {
        
        shared_ptr < RemoteMiner > remoteMiner = *remoteMinerIt;
        if ( !remoteMiner->mTag.hasCursor ()) continue;
        
        this->mBlockTree->tag ( remoteMiner->mImproved, this->improveBranch ( remoteMiner->mImproved, ( *remoteMiner->mTag ).trimInvalid (), now ));
        if (( *remoteMiner->mImproved ).isMissing ()) continue;
        
        assert ( !( *remoteMiner->mImproved ).isMissingOrInvalid ());
        
        if ( BlockTreeCursor::compare ( *remoteMiner->mImproved, *this->mBestBranchTag, this->mRewriteMode ) < 0 ) {
            this->mBlockTree->tag ( this->mBestBranchTag, *remoteMiner->mImproved );
        }
    }
    assert ( this->mBestBranchTag.hasCursor ());
    assert ( !( *this->mBestBranchTag ).isMissingOrInvalid ());
}

//----------------------------------------------------------------//
void Miner::updateBlockSearches () {

    set < shared_ptr < RemoteMiner >>::const_iterator remoteMinerIt = this->mOnlineMiners.begin ();
    for ( ; remoteMinerIt != this->mOnlineMiners.end (); ++remoteMinerIt ) {
        
        shared_ptr < const RemoteMiner > remoteMiner = *remoteMinerIt;

        // we only care about missing branches; ignore new/complete/invalid branches.
        if ( remoteMiner->mImproved.hasCursor () && ( *remoteMiner->mImproved ).isMissing ()) {
            
            // only affirm a search if the other chain could beat our current.
            if ( BlockTreeCursor::compare ( *remoteMiner->mImproved, *this->mBestBranchTag, this->mRewriteMode ) < 0 ) {
                this->affirmBranchSearch ( *remoteMiner->mImproved );
            }
        }
    }
    
    // always affirm a search for the current branch
    this->affirmBranchSearch ( *this->mBestBranchTag );
    
    // step the currently active block searches
    map < string, BlockSearch >::iterator blockSearchIt = this->mBlockSearches.begin ();
    while ( blockSearchIt != this->mBlockSearches.end ()) {
    
        map < string, BlockSearch >::iterator cursor = blockSearchIt++;
    
        BlockSearch& blockSearch = cursor->second;
        if ( !blockSearch.step ( *this )) {
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
void Miner::updateRemoteMiners () {

    this->mOnlineMiners.clear ();
    this->mActiveMinerURLs.clear ();
    
    map < string, shared_ptr < RemoteMiner >>::iterator remoteMinerIt = this->mRemoteMinersByURL.begin ();
    for ( ; remoteMinerIt != this->mRemoteMinersByURL.end (); ++remoteMinerIt ) {
    
        shared_ptr < RemoteMiner > remoteMiner = remoteMinerIt->second;
        if ( remoteMiner->mNetworkState != RemoteMiner::STATE_ONLINE ) continue;
        
        this->mOnlineMiners.insert ( remoteMiner );;
        
        remoteMiner->updateHeaders ( *this->mBlockTree );
        
        if ( remoteMiner->mTag.hasCursor ()) {
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

    const MiningMessengerRequest& request   = response.mRequest;
    string url                              = response.mRequest.mMinerURL;
    MiningMessengerResponse::Status status  = response.mStatus;
    
    // TODO: these could be set deliberately as an attack; fix by sending up a nonce
    if ( url == this->mURL ) return;
    if ( response.mMinerID == this->mMinerID ) return;
    
    map < string, shared_ptr < RemoteMiner >>::iterator remoteMinerIt = this->mRemoteMinersByURL.find ( url );
    shared_ptr < RemoteMiner > remoteMiner = remoteMinerIt != this->mRemoteMinersByURL.cend () ? remoteMinerIt->second : NULL;
    
    switch ( request.mRequestType ) {
        
        case MiningMessengerRequest::REQUEST_BLOCK: {
            
            assert ( remoteMiner );
            this->mBlockTree->update ( response.mBlock );
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
                    if ( header->getDigest ().toHex () != this->mLedger->getGenesisHash ()) {
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
                    
                    remoteMiner = make_shared < RemoteMiner >();
                    remoteMiner->mURL = url;
                    remoteMiner->setMinerID ( response.mMinerID );
                    
                    this->mRemoteMinersByURL [ url ]                    = remoteMiner;
                    this->mRemoteMinersByID [ remoteMiner->getMinerID ()]   = remoteMiner;
                }

                assert ( remoteMiner->getMinerID () == response.mMinerID ); // TODO: handle this case
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
