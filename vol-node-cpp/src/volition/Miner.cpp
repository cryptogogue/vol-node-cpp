// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractChainRecorder.h>
#include <volition/Block.h>
#include <volition/Digest.h>
#include <volition/Miner.h>
#include <volition/MinerLaunchTests.h>
#include <volition/TheContext.h>
#include <volition/SyncChainTask.h>

namespace Volition {

//================================================================//
// RemoteMiner
//================================================================//

//----------------------------------------------------------------//
RemoteMiner::RemoteMiner () :
    mCurrentBlock ( 0 ),
    mWaitingForTask ( false ) {
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
const BlockTreeTag& Miner::getBlockTreeTag () const {

    return this->mTag;
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
time_t Miner::getTime () const {

    return this->Miner_getTime ();
}

//----------------------------------------------------------------//
const Signature& Miner::getVisage () const {

    return this->mVisage;
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
    mTaskManager ( this->mTaskManagerThreadPool ) {
    
    MinerLaunchTests::checkEnvironment ();
    
    this->mTaskManager.addObserver (
        Poco::Observer < Miner, Poco::TaskFinishedNotification > ( *this, &Miner::onSyncChainNotification )
    );
}

//----------------------------------------------------------------//
Miner::~Miner () {
}

//----------------------------------------------------------------//
void Miner::onSyncChainNotification ( Poco::TaskFinishedNotification* pNf ) {

    SyncChainTask* task = dynamic_cast < SyncChainTask* >( pNf->task ());
    if (( task ) && ( task->mBlockQueueEntry )) {
        Poco::ScopedLock < Poco::Mutex > chainMutexLock ( this->mMutex );
        this->mBlockQueue.push_back ( move ( task->mBlockQueueEntry ));
    }
    pNf->release ();
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
        RemoteMiner& remoteMiner = this->mRemoteMiners [ entry.mMinerID ];

        if ( entry.mBlock ) {

            remoteMiner.mCurrentBlock = entry.mBlock->getHeight ();
            remoteMiner.mTag = this->mBlockTree.affirmBlock ( entry.mBlock );

            if ( remoteMiner.mTag ) {
                remoteMiner.mCurrentBlock++; // next block
            }
            else {
                remoteMiner.mCurrentBlock--; // back up
            }
        }

        if ( this->mMinerSet.find ( entry.mMinerID ) != this->mMinerSet.end ()) {
            this->mMinerSet.erase ( entry.mMinerID );
        }

        remoteMiner.mWaitingForTask = false;
    }
}

//----------------------------------------------------------------//
void Miner::pushBlock ( shared_ptr < const Block > block ) {

    bool result = this->mChain->pushBlock ( *block, this->mBlockVerificationPolicy );
    assert ( result );
    
    this->mTag.mark ( this->mBlockTree.affirmBlock ( block ));
}

//----------------------------------------------------------------//
void Miner::rebuildChain ( shared_ptr < const BlockTreeNode > original, shared_ptr < const BlockTreeNode > replace ) {

    BlockTreeRoot root = BlockTreeNode::findRoot ( original, replace );

    this->mChain->reset (( root.mRoot ? root.mRoot->getHeight () : 0 ) + 1 );
    this->rebuildChainRecurse ( replace, root.mRoot );
}

//----------------------------------------------------------------//
void Miner::rebuildChainRecurse ( shared_ptr < const BlockTreeNode > node, shared_ptr < const BlockTreeNode > root ) {

    if ( node == root ) return;
    shared_ptr < const Block > block = node->getBlock ();
    if ( block->isGenesis ()) return;
    
    this->rebuildChainRecurse ( node->getParent (), root );
    bool result = this->mChain->pushBlock ( *node->getBlock (), this->mBlockVerificationPolicy );
    assert ( result );
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

    bool addToSet = ( this->mMinerSet.size () == 0 );

    map < string, RemoteMiner >::iterator remoteMinerIt = this->mRemoteMiners.begin ();
    for ( ; remoteMinerIt != this->mRemoteMiners.end (); ++remoteMinerIt ) {
    
        RemoteMiner& remoteMiner = remoteMinerIt->second;
        
        if ( !remoteMiner.mWaitingForTask ) {
            remoteMiner.mWaitingForTask = true;
            string url;
            Format::write ( url, "%sblocks/%d/", remoteMiner.mURL.c_str (), ( int )remoteMiner.mCurrentBlock );
            this->mTaskManager.start ( new SyncChainTask ( remoteMinerIt->first, url ));
        }
        
        if ( addToSet ) {
            this->mMinerSet.insert ( remoteMinerIt->first );
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
    
        this->processQueue ();
        
        shared_ptr < const BlockTreeNode > originalBranch = this->mTag;
        
        // find the best branch
        map < string, RemoteMiner >::const_iterator remoteMinerIt = this->mRemoteMiners.begin ();
        for ( ; remoteMinerIt != this->mRemoteMiners.end (); ++remoteMinerIt ) {
           const RemoteMiner& remoteMiner = remoteMinerIt->second;
           if ( remoteMiner.mTag &&  ( BlockTreeTag::compare ( remoteMiner.mTag, this->mTag ) < 0 )) {
                this->mTag = remoteMiner.mTag;
           }
        }
        
        if ( originalBranch != this->mTag.getNode ()) {
            this->rebuildChain ( originalBranch, this->mTag );
        }
    
        if ( this->mRemoteMiners.size () && ( this->mTag.getCount () > ( this->mRemoteMiners.size () >> 1 ))) {
            this->extend ();
        }

        this->discoverMiners ();
        this->startTasks ();
    }
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

//----------------------------------------------------------------//
void Miner::Miner_reset () {
}


//----------------------------------------------------------------//
void Miner::Miner_shutdown ( bool kill ) {
    UNUSED ( kill );

    this->mTaskManager.cancelAll ();
    this->mTaskManager.joinAll ();
}

} // namespace Volition
