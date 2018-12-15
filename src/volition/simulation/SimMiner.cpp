// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Cohort.h>
#include <volition/TheContext.h>
#include <volition/simulation/SimMiner.h>
#include <volition/simulation/Simulation.h>
#include <volition/transactions/GenesisMiner.h>

namespace Volition {
namespace Simulation {

//================================================================//
// SimMiner
//================================================================//

//----------------------------------------------------------------//
const SimMiner* SimMiner::nextMiner () {

    if ( this->mMinerCursor < this->mMinerQueue.size ()) {
    
        int minerIdx = this->mMinerQueue [ this->mMinerCursor++ ];
        const SimMiner* miner = &this->mSimulation.getMiner ( minerIdx );
        return ( this == miner ) ? this->nextMiner () : miner;
    }
    return 0;
}

//----------------------------------------------------------------//
void SimMiner::log ( string prefix ) const {

    string name = this->mCohort ? this->mCohort->mName.c_str () : "";

    LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s[%s] %s", prefix.c_str (), name.c_str (), this->mChain.print ( this->mMetadata, "CHAIN: " ).c_str ());
}

//----------------------------------------------------------------//
void SimMiner::pushGenesisTransaction ( Block& block ) const {

    unique_ptr < Transactions::GenesisMiner > genesisMinerTransaction = make_unique < Transactions::GenesisMiner >();
    
    genesisMinerTransaction->mAccountName = this->mMinerID;
    genesisMinerTransaction->mKey = this->mKeyPair;
    genesisMinerTransaction->mKeyName = "master";
    genesisMinerTransaction->mAmount = 0;
    genesisMinerTransaction->mURL = "";

    block.pushTransaction ( move ( genesisMinerTransaction ));
}

//----------------------------------------------------------------//
void SimMiner::resetMinerQueue () {

    this->mSimulation.resetMinerQueue ( this->mMinerQueue, true );
    this->mMinerCursor = 0;
}

//----------------------------------------------------------------//
void SimMiner::step () {

    LGN_LOG_SCOPE ( VOL_FILTER_ROOT, INFO, "SimMiner::step ()" );

    int tag = ( int )( this->mSimulation.rand () & 0xffffffff );
    LGN_LOG ( VOL_FILTER_ROOT, INFO, "0x%08x", tag );

    if ( this->mCohort && this->mCohort->mIsPaused ) return;

    if ( this->mMinerCursor >= this->mMinerQueue.size ()) {
        this->resetMinerQueue ();
        this->pushBlock ( this->mChain, true );
    }
    
    const SimMiner* miner = this->nextMiner ();
    if ( !miner ) return;

    if ( this->mVerbose ) {
        LGN_LOG ( VOL_FILTER_ROOT, INFO, " player: %s\n", this->mMinerID.c_str ());
        LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s", this->mChain.print ( this->mMetadata, "   CHAIN0: " ).c_str ());
        LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s", miner->mChain.print ( this->mMetadata, "   CHAIN1: " ).c_str ());
    }

    this->updateChain ( miner->mChain );

    if ( this->mVerbose ) {
        LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s", this->mChain.print ( this->mMetadata, "     BEST: " ).c_str ());
        LGN_LOG ( VOL_FILTER_ROOT, INFO, "\n" );
    }
}

//----------------------------------------------------------------//
SimMiner::SimMiner ( Simulation& simulation ) :
    mCohort ( 0 ),
    mSimulation ( simulation ),
    mFrequency ( 1 ),
    mVerbose ( false ),
    mMinerCursor ( 0 ) {
    
    this->mKeyPair.elliptic ( CryptoKey::DEFAULT_EC_GROUP_NAME );
}

//----------------------------------------------------------------//
SimMiner::~SimMiner () {
}

} // namespace Simulator
} // namespace Volition
