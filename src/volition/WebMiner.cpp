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
            
            Chain::UpdateResult result = this->mChain.update ( this->mMetadata, entry.mBlock );
            
            switch ( result ) {
            
                case Chain::UpdateResult::UPDATE_ACCEPTED:
                case Chain::UpdateResult::UPDATE_EQUALS:
                    remoteMiner.mCurrentBlock++;
                    break;
                
                case Chain::UpdateResult::UPDATE_RETRY:
                    break;
                
                case Chain::UpdateResult::UPDATE_REWIND:
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
void WebMiner::runMulti () {

    size_t height = 0;
    while ( !this->isStopped ()) {
            
        Poco::ScopedLock < Poco::Mutex > chainMutexLock ( this->mMutex );
        
        LOG_SCOPE_F ( INFO, "WEB: WebMiner::runMulti () - step" );
        
        // process queue
        this->processQueue ();
        
        // push block (if we've heard from everybody)
        if ( this->mMinerSet.size () == 0 ) {
            this->pushBlock ( this->mChain, true );
        }
        
        // report chain
        size_t nextHeight = this->mChain.getVersion ();
        if ( nextHeight != height ) {
            LOG_F ( INFO, "WEB: height: %d", ( int )nextHeight );
            LOG_F ( INFO, "WEB.CHAIN: %s", this->mChain.print ( this->mMetadata ).c_str ());
            height = nextHeight;
            this->saveChain ();
        }
        
        // update remote miners
        this->updateMiners ();
        
        // kick off next batch of tasks
        this->startTasks ();

        Poco::Thread::sleep ( 200 );
    }
}

//----------------------------------------------------------------//
void WebMiner::runSolo () {

    size_t height = 0;
    while ( !this->isStopped ()) {
    
        LOG_SCOPE_F ( INFO, "WEB: WebMiner::runSolo () - step" );
        
        this->pushBlock ( this->mChain, true );
        
        size_t nextHeight = this->mChain.getVersion ();
        if ( nextHeight != height ) {
            LOG_F ( INFO, "WEB: height: %d", ( int )nextHeight );
            LOG_F ( INFO, "WEB.CHAIN: %s", this->mChain.print ( this->mMetadata ).c_str ());
            height = nextHeight;
            this->saveChain ();
        }
        Poco::Thread::sleep ( 200 );
    }
}

//----------------------------------------------------------------//
void WebMiner::setSolo ( bool solo ) {

    this->mSolo = solo;
}

//----------------------------------------------------------------//
void WebMiner::shutdown () {

    this->stop ();
    this->mTaskManager.joinAll ();
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

    map < string, MinerInfo > miners = this->mChain.getMiners ();
    
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
    Poco::Activity < WebMiner >( this, &WebMiner::run ),
    mSolo ( false ) {
    
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
void WebMiner::run () {

    if ( this->mSolo ) {
        this->runSolo ();
    }
    else {
        this->runMulti ();
    }
}

} // namespace Volition
