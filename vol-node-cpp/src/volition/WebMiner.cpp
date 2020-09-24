// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Format.h>
#include <volition/Singleton.h>
#include <volition/TheContext.h>
#include <volition/WebMiner.h>

namespace Volition {

//================================================================//
// WebMiner
//================================================================//

//----------------------------------------------------------------//
void WebMiner::runActivity () {

    while ( !this->isStopped ()) {
    
        Poco::Timestamp timestamp;
        
        this->step ();
        
        u32 elapsedMillis = ( u32 )( timestamp.elapsed () / 1000 );
        u32 updateMillis = this->mUpdateIntervalInSeconds * 1000;
        
        if ( elapsedMillis < updateMillis ) {
            Poco::Thread::sleep ( updateMillis - elapsedMillis );
        }
        Poco::Thread::sleep ( 5000 );
    }
}

//----------------------------------------------------------------//
void WebMiner::setUpdateInterval ( u32 updateIntervalInSeconds ) {

    this->mUpdateIntervalInSeconds = updateIntervalInSeconds;
}

//----------------------------------------------------------------//
void WebMiner::waitForShutdown () {

    this->mShutdownEvent.wait ();
}

//----------------------------------------------------------------//
WebMiner::WebMiner () :
    Poco::Activity < WebMiner >( this, &WebMiner::runActivity ),
    mUpdateIntervalInSeconds ( DEFAULT_UPDATE_INTERVAL ) {
}

//----------------------------------------------------------------//
WebMiner::~WebMiner () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void WebMiner::Miner_reset () {
}

//----------------------------------------------------------------//
void WebMiner::Miner_shutdown ( bool kill ) {

    if ( !this->isStopped ()) {
    
        this->Miner::Miner_shutdown ( kill );
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
