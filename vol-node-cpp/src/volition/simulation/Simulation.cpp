// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/TheContext.h>
#include <volition/simulation/Analysis.h>
#include <volition/simulation/Cohort.h>
#include <volition/simulation/SimMiner.h>
#include <volition/simulation/Simulation.h>
#include <volition/transactions/Genesis.h>

namespace Volition {
namespace Simulation {

//================================================================//
// Simulation
//================================================================//

//----------------------------------------------------------------//
bool Simulation::Simulation_control ( size_t step ) {
    UNUSED ( step );

    return true;
}

//----------------------------------------------------------------//
void Simulation::Simulation_report ( size_t step ) const {

    LGN_LOG ( VOL_FILTER_ROOT, INFO, "SIM: ROUND: %d - ", ( int )step );
    this->logTree ( "SIM: ", true, 1 );
    this->logMiners ( "SIM: " );
    LGN_LOG ( VOL_FILTER_ROOT, INFO, "SIM:" );
}

//----------------------------------------------------------------//
void Simulation::applyCohort ( Cohort& cohort, string name, size_t baseMinerID, size_t size ) {

    cohort.mName        = name;
    cohort.mBasePlayer  = baseMinerID;
    
    cohort.mMiners.clear ();
    for ( size_t i = 0; i < size; ++i ) {
        size_t idx = i + baseMinerID;
        this->mMiners [ idx ]->mCohort = &cohort;
        cohort.mMiners.push_back ( this->mMiners [ idx ].get ());
    }
}

//----------------------------------------------------------------//
size_t Simulation::countMiners () const {

    return this->mMiners.size ();
}

//----------------------------------------------------------------//
bool Simulation::drop () {

    return (( float )( this->rand () % ( 1000 + 1 )) / 1000.0 ) < this->mDropRate;
}

//----------------------------------------------------------------//
const Analysis& Simulation::getAnalysis () const {

    return this->mAnalysis;
}

//----------------------------------------------------------------//
const SimMiner& Simulation::getMiner ( size_t minerID ) const {

    return *this->mMiners [ minerID ];
}

//----------------------------------------------------------------//
void Simulation::initMiners ( size_t nMiners, TheContext::ScoringMode scoringMode, double window, u64 stepSize ) {

    TheContext::get ().setScoringMode ( scoringMode, nMiners );
    TheContext::get ().setWindow ( window );

    this->mStepSize = stepSize;

    if ( !this->mGenesisKey ) {
        this->mGenesisKey.elliptic ( CryptoKey::DEFAULT_EC_GROUP_NAME );
    }

    Block genesisBlock;

    this->mMiners.clear ();
    this->mMiners.resize ( nMiners );

    time_t now;
    time ( &now );

    unique_ptr < Transactions::Genesis > genesisMinerTransactionBody = make_unique < Transactions::Genesis >();
    genesisMinerTransactionBody->setIdentity ( "SIMULATION" );

    for ( size_t i = 0; i < nMiners; ++i ) {
        this->mMiners [ i ] = make_unique < SimMiner >( *this );
        SimMiner& miner = *this->mMiners [ i ];
        
        stringstream minerIDStream;
        minerIDStream << i;
        miner.setMinerID ( minerIDStream.str ());
        miner.setTime ( now );
        
        miner.pushGenesisAccount ( *genesisMinerTransactionBody );
    }
    
    shared_ptr < Transaction > transaction = make_shared < Transaction >();
    transaction->setBody ( move ( genesisMinerTransactionBody ));
    genesisBlock.pushTransaction ( transaction );
    
    for ( size_t i = 0; i < nMiners; ++i ) {
        this->mMiners [ i ]->setGenesis ( genesisBlock );
    }
}

//----------------------------------------------------------------//
void Simulation::logMiners ( string prefix ) const {

    size_t nMiners = this->countMiners ();
    for ( size_t i = 0; i < nMiners; ++i ) {
        this->mMiners [ i ]->log ( prefix );
    }
}

//----------------------------------------------------------------//
void Simulation::logTree ( string prefix, bool verbose, size_t maxDepth ) const {

    this->mAnalysis.log ( prefix, verbose, maxDepth );
}

//----------------------------------------------------------------//
size_t Simulation::rand () {

    return this->mRand ();
}

//----------------------------------------------------------------//
void Simulation::reset () {

    this->mMiners.clear ();
    this->mRand.seed ( 1 );
    
    this->mDropRate           = 0.0;
    this->mCyclesPerStep      = 1;
    this->mRandomizeScore     = false;
}

//----------------------------------------------------------------//
void Simulation::resetMinerQueue ( vector < size_t >& minerQueue, bool shuffle ) {

    size_t nMiners = this->countMiners ();

    minerQueue.clear ();
    minerQueue.reserve ( nMiners );
    
    for ( size_t i = 0; i < nMiners; ++i ) {
        minerQueue.push_back ( i );
    }
    
    if ( shuffle ) {
        std::shuffle ( minerQueue.begin (), minerQueue.end (), this->mRand );
    }
}

//----------------------------------------------------------------//
void Simulation::run () {

    for ( size_t i = 0; this->Simulation_control ( i ); ++i ) {
        this->step ( i );
    }
}

//----------------------------------------------------------------//
void Simulation::run ( size_t iterations, bool force ) {

    for ( size_t i = 0; (( i < iterations ) && ( force || this->Simulation_control ( i ))); ++i ) {
        this->step ( i );
    }
}

//----------------------------------------------------------------//
void Simulation::setCyclesPerStep ( size_t cycles ) {

    this->mCyclesPerStep = cycles;
}

//----------------------------------------------------------------//
void Simulation::setDropRate ( double percentage ) {

    this->mDropRate = percentage;
}

//----------------------------------------------------------------//
void Simulation::setPlayerVerbose ( size_t minerID, bool verbose ) {

    this->mMiners [ minerID ]->mVerbose = verbose;
}

//----------------------------------------------------------------//
void Simulation::setScoreRandomizer ( bool randomize ) {

    this->mRandomizeScore = randomize;
}

//----------------------------------------------------------------//
void Simulation::setStepSize ( u64 stepSize ) {

    this->mStepSize = stepSize;
}

//----------------------------------------------------------------//
Simulation::Simulation () :
    mStepSize ( 0 ) {
}

//----------------------------------------------------------------//
Simulation::~Simulation () {
}

//----------------------------------------------------------------//
void Simulation::step ( size_t step ) {
    UNUSED ( step );

    size_t nMiners = this->countMiners ();
    size_t cycles = this->mCyclesPerStep ? this->mCyclesPerStep : 1;

    for ( size_t i = 0; i < cycles; ++i ) {

        map < SimMiner*, size_t > schedule;

        for ( size_t j = 0; j < nMiners; ++j ) {
            SimMiner& miner = *this->mMiners [ j ];
            schedule [ &miner ] = miner.mFrequency;
            miner.update_extend ();
        }

        while ( schedule.size ()) {

            map < SimMiner*, size_t >::iterator scheduleIt = next ( schedule.begin (), ( int )( this->rand () % schedule.size ()));

            SimMiner* miner = scheduleIt->first;
            if ( !this->drop ()) {
                miner->update_select ();
            }
            miner->step ( this->mStepSize ); // advance time

            scheduleIt->second -= 1;
            if ( scheduleIt->second <= 0 ) {
                schedule.erase ( scheduleIt );
            }
        }
    }
    
    Tree tree;
    for ( size_t i = 0; i < nMiners; ++i ) {
        tree.addChain ( *this->mMiners [ i ]->getBestBranch ());
    }
    this->mAnalysis.update ( tree );
//    this->Simulation_report ( step );
}

} // namespace Simulator
} // namespace Volition

