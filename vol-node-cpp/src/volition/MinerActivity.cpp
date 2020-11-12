// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Format.h>
#include <volition/Singleton.h>
#include <volition/MinerActivity.h>

namespace Volition {

//================================================================//
// MinerActivity
//================================================================//

//----------------------------------------------------------------//
void MinerActivity::runActivity () {

    while ( !this->isStopped ()) {
    
//        Poco::Timestamp timestamp;
        
        time_t now;
        time ( &now );
        
        try {
            this->step ( now );
        }
        catch ( Poco::Exception& exc ) {
        
            printf ( "an exception\n" );
        }
        
//        u32 elapsedMillis = ( u32 )( timestamp.elapsed () / 1000 );
//        u32 updateMillis = this->mUpdateIntervalInSeconds * 1000;
//
//        if ( elapsedMillis < updateMillis ) {
//            Poco::Thread::sleep ( updateMillis - elapsedMillis );
//        }
//        Poco::Thread::sleep ( 5000 );
        Poco::Thread::sleep ( 100 );
    }
}

//----------------------------------------------------------------//
void MinerActivity::setUpdateInterval ( u32 updateIntervalInSeconds ) {

    this->mUpdateIntervalInSeconds = updateIntervalInSeconds;
}

//----------------------------------------------------------------//
void MinerActivity::waitForShutdown () {

    this->mShutdownEvent.wait ();
}

//----------------------------------------------------------------//
MinerActivity::MinerActivity () :
    Poco::Activity < MinerActivity >( this, &MinerActivity::runActivity ),
    mUpdateIntervalInSeconds ( DEFAULT_UPDATE_INTERVAL ) {
}

//----------------------------------------------------------------//
MinerActivity::~MinerActivity () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void MinerActivity::Miner_reset () {
}

//----------------------------------------------------------------//
void MinerActivity::Miner_shutdown ( bool kill ) {

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
