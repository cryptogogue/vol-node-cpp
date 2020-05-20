// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Format.h>
#include <volition/SyncChainTask.h>
#include <volition/TheContext.h>
#include <volition/WebMiner.h>

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

        if ( entry.mHasBlock ) {

            remoteMiner.mCurrentBlock = entry.mBlock.getHeight ();

            SubmissionResponse result = this->submitBlock ( entry.mBlock );

            switch ( result ) {

                case SubmissionResponse::ACCEPTED:
                    remoteMiner.mCurrentBlock++;
                    break;

                case SubmissionResponse::RESUBMIT_EARLIER:
                    if ( remoteMiner.mCurrentBlock > 0 ) {
                        remoteMiner.mCurrentBlock--;
                    }
                    break;

                default:
                    assert ( false );
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

    if ( this->mSolo ) {
        this->runSolo ();
    }
    else {
        this->runMulti ();
    }
}

//----------------------------------------------------------------//
void WebMiner::runMulti () {

    size_t height = 0;
    while ( !this->isStopped ()) {
        {
            Poco::ScopedLock < Poco::Mutex > scopedLock ( this->mMutex );

            LGN_LOG_SCOPE ( VOL_FILTER_ROOT, INFO, "WEB: WebMiner::runMulti () - step" );

            // process queue
            this->processQueue ();
            this->selectBranch ();
            this->extend ( this->mMinerSet.size () == 0 ); // force push if we processed all others

            // report chain
            const Chain& chain = *this->getBestBranch ();
            size_t nextHeight = chain.countBlocks ();
            if ( nextHeight != height ) {
                LGN_LOG ( VOL_FILTER_ROOT, INFO, "WEB: height: %d", ( int )nextHeight );
                LGN_LOG ( VOL_FILTER_ROOT, INFO, "WEB.CHAIN: %s", chain.print ().c_str ());
                height = nextHeight;
                this->saveChain ();
            }

            // update remote miners
            this->updateMiners ();

            // kick off next batch of tasks
            this->startTasks ();
        }
        Poco::Thread::sleep ( 200 );
    }
}

//----------------------------------------------------------------//
void WebMiner::runSolo () {

    size_t height = 0;
    while ( !this->isStopped ()) {
    
        Poco::Timestamp timestamp;
    
        {
            Poco::ScopedLock < Poco::Mutex > scopedLock ( this->mMutex );

            this->extend ( true );

            const Chain& chain = *this->getBestBranch ();
            size_t nextHeight = chain.countBlocks ();
            if ( nextHeight != height ) {
                LGN_LOG_SCOPE ( VOL_FILTER_ROOT, INFO, "WEB: WebMiner::runSolo () - step" );
                LGN_LOG ( VOL_FILTER_ROOT, INFO, "WEB: height: %d", ( int )nextHeight );
                LGN_LOG ( VOL_FILTER_ROOT, INFO, "WEB.CHAIN: %s", chain.print ().c_str ());
                height = nextHeight;
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
void WebMiner::shutdown () {

    this->mTaskManager.cancelAll ();
    this->mTaskManager.joinAll ();
    this->stop ();
    this->wait ();
}

//----------------------------------------------------------------//
void WebMiner::startTasks () {

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
void WebMiner::updateMiners () {

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
WebMiner::WebMiner () :
    Poco::Activity < WebMiner >( this, &WebMiner::runActivity ),
    mTaskManager ( this->mTaskManagerThreadPool ),
    mSolo ( false ),
    mUpdateIntervalInSeconds ( DEFAULT_UPDATE_INTERVAL ) {
    
    this->mTaskManager.addObserver (
        Poco::Observer < WebMiner, Poco::TaskFinishedNotification > ( *this, &WebMiner::onSyncChainNotification )
    );
}

//----------------------------------------------------------------//
WebMiner::~WebMiner () {
}

} // namespace Volition
