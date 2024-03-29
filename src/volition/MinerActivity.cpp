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
MinerActivity::MinerActivity () :
    Poco::Activity < MinerActivity >( this, &MinerActivity::runActivity ),
    mFixedUpdateDelayInMillis ( DEFAULT_FIXED_UPDATE_MILLIS ),
    mVariableUpdateDelayInMillis ( DEFAULT_VARIABLE_UPDATE_MILLIS ) {
}

//----------------------------------------------------------------//
MinerActivity::~MinerActivity () {
}

//----------------------------------------------------------------//
void MinerActivity::runActivity () {

    this->mSnapshot = *this;

    while ( !this->isStopped ()) {
        
        Lognosis::rotateLogFiles ();
        
        Poco::Timestamp timestamp;
        
        time_t now;
        time ( &now );
        
        try {
            this->mMutex.lock ();
            this->mBlockTreeMutex.lock ();
            
            this->step ( now );
            this->report ();
            this->mBlockSearchPool->reportBlockSearches ();
            
            this->mBlockTreeMutex.unlock ();
            this->mMutex.unlock ();
        }
        catch ( const Poco::Exception& exc ) {
            LGN_LOG ( VOL_FILTER_CONSENSUS, INFO, "Caught exception in MinerActivity::runActivity ()" );
            LGN_LOG ( VOL_FILTER_CONSENSUS, INFO, "%s", exc.displayText ().c_str ());
        }
        
        u32 elapsedMillis = ( u32 )( timestamp.elapsed () / 1000 );
        u32 updateMillis = this->mVariableUpdateDelayInMillis;

        if ( elapsedMillis < updateMillis ) {
            Poco::Thread::sleep ( updateMillis - elapsedMillis );
        }
        Poco::Thread::sleep ( this->mFixedUpdateDelayInMillis ? this->mFixedUpdateDelayInMillis : 1 );
    }
}

//----------------------------------------------------------------//
void MinerActivity::waitForShutdown () {

    this->mShutdownEvent.wait ();
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
