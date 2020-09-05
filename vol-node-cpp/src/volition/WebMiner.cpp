// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Format.h>
#include <volition/Singleton.h>
#include <volition/SyncChainTask.h>
#include <volition/TheContext.h>
#include <volition/WebMiner.h>

// Axioms
// 1. SELF must agree with at least N other miners in the last M samples (where N and M may be adjusted by total active miners).
// 2. ALLURE for block N is *not* affected by previous blocks. (why not?)
// 3. there is always one (and only one) IDEAL chain. (chain with the greatest per-block allure.)
// 4. Any chain may be compared against any other and scored by their relation to the IDEAL.
//      a. It is possible for multiple chains to "tie" in scoring.
//      b. To break a tie, pick the chain with the most recent willing ALLURE.
// 5. A chain produced by more active miners will eventually beat a chain produced by fewer.
//      a. The larger pool of miners has more ALLURE to choose from, and thus is more likely to have the winning ALLUREs.

// When initially building the chain, we only need block headers (with allure).
// To compare branches, we do not have to apply any transactions.
// Once we've committed to a branch, we can request the full block.
// If a block delivery times out, discard the chain

// conceptually, always get the full chain from each node sampled
// broadcast a change as soon as detected (after evaluating batch of samples)
// only advance when consensus threshold is met and blocks are known
// roll back as soon as a better branch is proven
//      branches have to be long enough to compete
//      check headers first - if branch would lose, don't roll back
//      use waypoints to efficiently seek back to branch point
//      after seek back, gather block headers moving forward from fork
//      maintain rollback branches as competitors until proven
//      during seek, detect if branch changes and restart if unrecognized header

// always get the whole chain
// RACE AHEAD when a point of consensus is found (N of last M random samples AND blocks are known)
// REWIND as soon as a better chain is proven (and blocks are known)
// AS SOON as a new leaf is found (with an unknown root), seek back to fork then rebuild (and evaluate)
// ignore shorter branches that CANNOT WIN

// if there is an OUTAGE (more than a certain number of nodes stop responding), network should HALT

// when adding or removing a miner a hard FORK is created
// the name of the FORK must be unique and be adopted by the remaining miners

// REMOTE MINERs cache chain FRAGMENTs
// ask REMOTE MINER for FRAGMENT starting at HEIGHT
// FRAGMENT may be incomplete
// if FRAGMENT doesn't have a KNOWN ROOT, remote chain has to be rewound

// we'll keep a tree of block headers that may or may not have full blocks attached
// as chain fragments come in, we'll add them to the tree and mark branches for evaluation
// active miners have to "pin" tree branches; prune branches with no miners
// miners get updated from the service response queue

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
// WebMiner
//================================================================//

//----------------------------------------------------------------//
Poco::Mutex& WebMiner::getMutex () {

    return this->mMutex;
}

//----------------------------------------------------------------//
SerializableTime WebMiner::getStartTime () {

    return this->mStartTime;
}

//----------------------------------------------------------------//
void WebMiner::onSyncChainNotification ( Poco::TaskFinishedNotification* pNf ) {

    SyncChainTask* task = dynamic_cast < SyncChainTask* >( pNf->task ());
    if (( task ) && ( task->mBlockQueueEntry )) {
        Poco::ScopedLock < Poco::Mutex > chainMutexLock ( this->mMutex );
        this->mBlockQueue.push_back ( move ( task->mBlockQueueEntry ));
    }
    pNf->release ();
}

//----------------------------------------------------------------//
void WebMiner::processQueue () {

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
void WebMiner::runActivity () {

    this->mHeight = 0;
    while ( !this->isStopped ()) {
    
        Poco::Timestamp timestamp;
        {
            Poco::ScopedLock < Poco::Mutex > scopedLock ( this->mMutex );

            this->step ();

            const Chain& chain = *this->getBestBranch ();
            size_t nextHeight = chain.countBlocks ();
            if ( nextHeight != this->mHeight ) {
//                LGN_LOG_SCOPE ( VOL_FILTER_ROOT, INFO, "WEB: WebMiner::runSolo () - step" );
//                LGN_LOG ( VOL_FILTER_ROOT, INFO, "WEB: height: %d", ( int )nextHeight );
//                LGN_LOG ( VOL_FILTER_ROOT, INFO, "WEB.CHAIN: %s", chain.print ().c_str ());
                this->mHeight = nextHeight;
                this->saveChain ();
                this->pruneTransactions ( chain );
            }
        }
        
        u32 elapsedMillis = ( u32 )( timestamp.elapsed () / 1000 );
        u32 updateMillis = this->mUpdateIntervalInSeconds * 1000;
        
        if ( elapsedMillis < updateMillis ) {
            Poco::Thread::sleep ( updateMillis - elapsedMillis );
        }
        Poco::Thread::sleep ( 5000 );
        
        this->processIncoming ( *this );
    }
}

//----------------------------------------------------------------//
void WebMiner::setSolo ( bool solo ) {

    this->mSolo = solo;
}

//----------------------------------------------------------------//
void WebMiner::setUpdateInterval ( u32 updateIntervalInSeconds ) {

    this->mUpdateIntervalInSeconds = updateIntervalInSeconds;
}

//----------------------------------------------------------------//
void WebMiner::startTasks () {

    map < string, MinerInfo > miners = this->getBestBranch ()->getMiners ();
        
    map < string, MinerInfo >::iterator minerIt = miners.begin ();
    for ( ; minerIt != miners.end (); ++minerIt ) {
        MinerInfo& minerInfo = minerIt->second;
        if ( minerIt->first != this->mMinerID ) {
            this->mRemoteMiners [ minerIt->first ].mURL = minerInfo.getURL (); // affirm
        }
    }

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
void WebMiner::step () {

    this->processIncoming ( *this );
    
    if ( this->mSolo ) {
        this->extend ( true );
    }
    else {
        this->processQueue ();
        
        bool rebuild = false;
        
        // find the best branch
        map < string, RemoteMiner >::const_iterator remoteMinerIt = this->mRemoteMiners.begin ();
        for ( ; remoteMinerIt != this->mRemoteMiners.end (); ++remoteMinerIt ) {
           const RemoteMiner& remoteMiner = remoteMinerIt->second;
           if ( remoteMiner.mTag && ( BlockTreeTag::compare ( remoteMiner.mTag, this->mTag ) < 0 )) {
               this->mTag = remoteMiner.mTag;
               rebuild = true;
           }
        }
        
        if ( rebuild ) {
            this->rebuildChain ();
        }
        
        if ( this->mTag.getCount () > ( this->mRemoteMiners.size () >> 1 )) {
            shared_ptr < Block > block = this->prepareBlock ();
            if ( block ) {
                this->pushBlock ( block );
            }
        }
        
        this->startTasks ();
    }
}

//----------------------------------------------------------------//
void WebMiner::waitForShutdown () {

    this->mShutdownEvent.wait ();
}

//----------------------------------------------------------------//
WebMiner::WebMiner () :
    Poco::Activity < WebMiner >( this, &WebMiner::runActivity ),
    mTaskManager ( this->mTaskManagerThreadPool ),
    mSolo ( false ),
    mUpdateIntervalInSeconds ( DEFAULT_UPDATE_INTERVAL ),
    mHeight ( 0 ) {
    
    this->mTaskManager.addObserver (
        Poco::Observer < WebMiner, Poco::TaskFinishedNotification > ( *this, &WebMiner::onSyncChainNotification )
    );
}

//----------------------------------------------------------------//
WebMiner::~WebMiner () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void WebMiner::Miner_reset () {

    this->mHeight = 0;
}

//----------------------------------------------------------------//
void WebMiner::Miner_shutdown ( bool kill ) {

    if ( !this->isStopped ()) {
        this->mTaskManager.cancelAll ();
        this->mTaskManager.joinAll ();
        this->stop ();
        
        if ( kill ) {
            printf ( "REQUESTED WEB MINER SHUTDOWN\n" );
            this->mShutdownEvent.set ();
            Poco::Util::ServerApplication::terminate ();
        }
    }
    
    if ( !kill ) {
        this->wait ();
    }
}

} // namespace Volition
