// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/SyncChainTask.h>
#include <volition/TheContext.h>
#include <volition/WebMiner.h>

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

    //size_t count = 0;
    size_t height = 0;
    while ( !this->isStopped ()) {
    
        if ( this->mChain && ( this->mMinerURLs.size () == 0 )) {
            
            this->pushBlock ( *this->mChain, true );
            //this->mChain->print ();
            
            if ( !this->mSolo ) {
                this->mMinerURLs = this->mChain->getMinerURLs ();
            }
            
            if ( this->mMinerURLs.find ( this->mMinerID ) != this->mMinerURLs.end ()) {
                this->mMinerURLs.erase ( this->mMinerID );
            }
            
            map < string, string >::iterator urlIt = this->mMinerURLs.begin ();
            for ( ; urlIt != this->mMinerURLs.end (); ++urlIt ) {
                string url = urlIt->second + ( "blocks/" );
                this->mTaskManager.start ( new SyncChainTask ( urlIt->first, url ));
            }
            
            size_t nextHeight = this->mChain->getVersion ();
            if ( nextHeight != height ) {
                printf ( "height: %d\n", ( int )nextHeight );
                this->mChain->print ();
                height = nextHeight;
            }
        }
        //printf ( "%d\n", ( unsigned int )count++ );
        Poco::Thread::sleep ( 200 );
    }
}

} // namespace Volition
