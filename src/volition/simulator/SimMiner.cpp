// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Cohort.h>
#include <volition/TheContext.h>
#include <volition/simulator/SimMiner.h>
#include <volition/simulator/TheSimulator.h>
#include <volition/Transactions.h>

namespace Volition {
namespace Simulator {

//================================================================//
// SimMiner
//================================================================//

//----------------------------------------------------------------//
const SimMiner* SimMiner::nextMiner () {

    if ( this->mMinerCursor < this->mMinerQueue.size ()) {
    
        int minerIdx = this->mMinerQueue [ this->mMinerCursor++ ];
        const SimMiner* miner = &TheSimulator::get ().getMiner ( minerIdx );
        return ( this == miner ) ? this->nextMiner () : miner;
    }
    return 0;
}

//----------------------------------------------------------------//
void SimMiner::pushGenesisTransaction ( Block& block ) const {

    unique_ptr < Transaction::GenesisMiner > genesisMinerTransaction = make_unique < Transaction::GenesisMiner >();
    
    genesisMinerTransaction->mAccountName = this->mMinerID;
    genesisMinerTransaction->mKey = this->mKeyPair;
    genesisMinerTransaction->mKeyName = "master";
    genesisMinerTransaction->mAmount = 0;
    genesisMinerTransaction->mURL = "";

    block.pushTransaction ( move ( genesisMinerTransaction ));
}

//----------------------------------------------------------------//
void SimMiner::print () const {

    string name = this->mCohort ? this->mCohort->mName.c_str () : "";

    LOG_F ( INFO, "[%s] %s", name.c_str (), this->mChain->print ( this->mMetadata, "CHAIN: " ).c_str ());
}

//----------------------------------------------------------------//
void SimMiner::resetMinerQueue () {

    TheSimulator::get ().resetMinerQueue ( this->mMinerQueue, true );
    this->mMinerCursor = 0;
}

//----------------------------------------------------------------//
void SimMiner::step () {

    LOG_SCOPE_F ( INFO, "SimMiner::step ()" );

    int tag = ( int )( TheSimulator::get ().rand () & 0xffffffff );
    LOG_F ( INFO, "0x%08x", tag );
    
    if ( tag == 0x62eca207 ) {
        LOG_F ( INFO, "break" );
    }
    
    if ( tag == 0x2fd078d7 ) {
        LOG_F ( INFO, "break" );
    }

    if ( this->mMinerCursor >= this->mMinerQueue.size ()) {
        this->resetMinerQueue ();
        this->pushBlock ( *this->mChain, true );
    }
    
    const SimMiner* miner = this->nextMiner ();
    if ( !miner ) return;

    //unique_ptr < Chain > chain = make_unique < Chain >( *miner->mChain );

    if ( this->mVerbose ) {
        LOG_F ( INFO, " player: %s\n", this->mMinerID.c_str ());
        LOG_F ( INFO, "%s", this->mChain->print ( this->mMetadata, "   CHAIN0: " ).c_str ());
        LOG_F ( INFO, "%s", miner->mChain->print ( this->mMetadata, "   CHAIN1: " ).c_str ());
    }

    this->updateChain ( *miner->mChain );

    LOG_F ( INFO, "%s", this->mChain->print ( this->mMetadata, "     TEMP: " ).c_str ());
    this->pushBlock ( *this->mChain, false );

    if ( this->mVerbose ) {
        LOG_F ( INFO, "%s", this->mChain->print ( this->mMetadata, "     BEST: " ).c_str ());
        LOG_F ( INFO, "\n" );
    }
}

//----------------------------------------------------------------//
SimMiner::SimMiner () :
    mCohort ( 0 ),
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
