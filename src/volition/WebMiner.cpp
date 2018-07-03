// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "AbstractHashable.h"
#include "Block.h"
#include "Genesis.h"
#include "SyncChainTask.h"
#include "WebMiner.h"

namespace Volition {

//================================================================//
// WebMiner
//================================================================//

//----------------------------------------------------------------//
void WebMiner::onSyncChainNotification ( Poco::TaskFinishedNotification* pNf ) {

    SyncChainTask* task = dynamic_cast < SyncChainTask* >( pNf->task ());
    if ( task ) {
        
        string minerID      = task->getMinerID ();
        string url          = task->getURL ();
        
        assert ( this->mMinerURLs.find ( minerID ) != this->mMinerURLs.end ());
        this->mMinerURLs.erase ( minerID );
        
        this->updateChain ( task->moveChain ());
    }
    pNf->release ();
}

//----------------------------------------------------------------//
void WebMiner::shutdown () {

    this->stop ();
    this->mTaskManager.joinAll ();
    this->wait ();
}

//----------------------------------------------------------------//
WebMiner::WebMiner () :
    Poco::Activity < WebMiner >( this, &WebMiner::run ) {
    
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

    size_t count = 0;
    while ( !this->isStopped ()) {
    
        if ( this->mChain && ( this->mMinerURLs.size () == 0 )) {
            
            this->pushBlock ( *this->mChain, true );
            this->mChain->print ();
            
            this->mMinerURLs = this->mState.getMinerURLs ();
            
            if ( this->mMinerURLs.find ( this->mMinerID ) != this->mMinerURLs.end ()) {
                this->mMinerURLs.erase ( this->mMinerID );
            }
            
            map < string, string >::iterator urlIt = this->mMinerURLs.begin ();
            for ( ; urlIt != this->mMinerURLs.end (); ++urlIt ) {
                this->mTaskManager.start ( new SyncChainTask ( urlIt->first, urlIt->second ));
            }
        }
        printf ( "%d\n", ( unsigned int )count++ );
        Poco::Thread::sleep ( 200 );
    }
}

} // namespace Volition
