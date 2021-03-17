// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/BlockSearchPool.h>
#include <volition/Format.h>
#include <volition/Singleton.h>
#include <volition/MinerActivity.h>

namespace Volition {

//================================================================//
// MinerActivity
//================================================================//

//----------------------------------------------------------------//
void MinerActivity::runActivity () {

    this->mSnapshot = *this;

    while ( !this->isStopped ()) {
        
        Poco::Timestamp timestamp;
        
        time_t now;
        time ( &now );
        
        try {
            Poco::ScopedLock < Poco::Mutex > scopedLock ( this->mMutex );
            this->step ( now );
            this->report ();
            this->mBlockSearchPool->reportBlockSearches ();
        }
        catch ( Poco::Exception& exc ) {
            LGN_LOG ( VOL_FILTER_CONSENSUS, INFO, "Caught exception in MinerActivity::runActivity ()" );
        }
        
        u32 elapsedMillis = ( u32 )( timestamp.elapsed () / 1000 );
        u32 updateMillis = this->mVariableUpdateDelayInMillis;

        if ( elapsedMillis < updateMillis ) {
            Poco::Thread::sleep ( updateMillis - elapsedMillis );
        }
        Poco::Thread::sleep ( this->mFixedUpdateDelayInMillis );
    }
}

//----------------------------------------------------------------//
void MinerActivity::waitForShutdown () {

    this->mShutdownEvent.wait ();
}

//----------------------------------------------------------------//
MinerActivity::MinerActivity () :
    Poco::Activity < MinerActivity >( this, &MinerActivity::runActivity ),
    mFixedUpdateDelayInMillis ( DEFAULT_FIXED_UPDATE_MILLIS ),
    mVariableUpdateDelayInMillis ( DEFAULT_VARIABLE_UPDATE_MILLIS ) {
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
            LGN_LOG ( VOL_FILTER_CONSENSUS, INFO, "REQUESTED WEB MINER SHUTDOWN\n" );
            this->mShutdownEvent.set ();
            Poco::Util::ServerApplication::terminate ();
        }
    }
    
    if ( !kill ) {
        this->wait ();
    }
}

} // namespace Volition
