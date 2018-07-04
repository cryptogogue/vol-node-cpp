// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <Genesis.h>
#include <simulator/SimMiner.h>
#include <simulator/TheSimulator.h>
#include <transactions/OpenAccount.h>
#include <transactions/RegisterMiner.h>

namespace Volition {
namespace Simulator {

//================================================================//
// SimMiner
//================================================================//

//----------------------------------------------------------------//
const SimMiner* SimMiner::nextMiner () {

    if ( this->mMinerCursor < this->mMinerQueue.size ()) {
    
        int minerIdx = this->mMinerQueue [ this->mMinerCursor++ ];
        return &TheSimulator::get ().getMiner ( minerIdx );
    }
    return 0;
}

//----------------------------------------------------------------//
void SimMiner::pushGenesis ( Block& block ) {

    block.pushTransaction (
        make_unique < Transaction::OpenAccount >(
            this->mMinerID,
            "master",
            *this->mKeyPair,
            0
        )
    );
    
    block.pushTransaction (
        make_unique < Transaction::RegisterMiner >(
            this->mMinerID,
            "",
            this->mMinerID + ".master"
        )
    );
}

//----------------------------------------------------------------//
void SimMiner::print () const {

    //printf ( "[%s] ", this->mCohort ? this->mCohort->mName.c_str () : "" );
    this->mChain->print ();
}

//----------------------------------------------------------------//
void SimMiner::resetMinerQueue () {

    TheSimulator::get ().resetMinerQueue ( this->mMinerQueue, true );
    this->mMinerCursor = 0;
}

//----------------------------------------------------------------//
void SimMiner::step () {

    if ( this->mMinerCursor >= this->mMinerQueue.size ()) {
        this->resetMinerQueue ();
        this->pushBlock ( *this->mChain, true );
    }
    
    const SimMiner* miner = this->nextMiner ();
    if ( !miner ) return;

    unique_ptr < Chain > chain = make_unique < Chain >( *miner->mChain );

    if ( this->mVerbose ) {
        printf ( " player: %s\n", this->mMinerID.c_str ());
        this->mChain->print ( "   CHAIN0: " );
        chain->print ( "   CHAIN1: " );
    }

    this->updateChain ( move ( chain ));

    if ( this->mVerbose ) {
        this->mChain->print ( "     BEST: " );
        printf ( "\n" );
    }
}

//----------------------------------------------------------------//
SimMiner::SimMiner () :
    mCohort ( 0 ),
    mFrequency ( 0 ),
    mVerbose ( false ),
    mMinerCursor ( 0 ) {
    
    this->mKeyPair = make_unique < Poco::Crypto::ECKey >( Genesis::EC_CURVE );
}

//----------------------------------------------------------------//
SimMiner::~SimMiner () {
}

} // namespace Simulator
} // namespace Volition
