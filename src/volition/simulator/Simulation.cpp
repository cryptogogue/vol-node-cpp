// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/TheContext.h>
#include <volition/simulator/Analysis.h>
#include <volition/simulator/Cohort.h>
#include <volition/simulator/SimMiner.h>
#include <volition/simulator/Simulation.h>

namespace Volition {
namespace Simulator {

//================================================================//
// Simulation
//================================================================//

//----------------------------------------------------------------//
bool Simulation::Simulation_control ( size_t step ) {

    return true;
}

//----------------------------------------------------------------//
void Simulation::Simulation_report ( size_t step ) const {

    LOG_F ( INFO, "SIM: ROUND: %d - ", ( int )step );
    this->logTree ( "SIM: ", false, 1 );
    this->logMiners ( "SIM: " );
    LOG_F ( INFO, "SIM:" );
}

//----------------------------------------------------------------//
void Simulation::applyCohort ( Cohort& cohort, string name, int baseMinerID, int size ) {

    cohort.mName        = name;
    cohort.mBasePlayer  = baseMinerID;
    
    cohort.mMiners.clear ();
    for ( int i = 0; i < size; ++i ) {
        int idx = i + baseMinerID;
        this->mMiners [ idx ]->mCohort = &cohort;
        cohort.mMiners.push_back ( this->mMiners [ idx ].get ());
    }
}

//----------------------------------------------------------------//
int Simulation::countMiners () const {

    return ( int )this->mMiners.size ();
}

//----------------------------------------------------------------//
bool Simulation::drop () {

    return (( float )( this->rand () % ( 1000 + 1 )) / 1000.0 ) < this->mDropRate;
}

//----------------------------------------------------------------//
const SimMiner& Simulation::getMiner ( int minerID ) const {

    return *this->mMiners [ minerID ];
}

//----------------------------------------------------------------//
void Simulation::initMiners ( int nMiners ) {

    if ( !this->mGenesisKey ) {
        this->mGenesisKey.elliptic ( CryptoKey::DEFAULT_EC_GROUP_NAME );
        TheContext::get ().setGenesisBlockKey ( this->mGenesisKey );
    }

    Block genesisBlock;

    this->mMiners.clear ();
    this->mMiners.resize ( nMiners );

    for ( int i = 0; i < nMiners; ++i ) {
        this->mMiners [ i ] = make_unique < SimMiner >( *this );
        SimMiner& miner = *this->mMiners [ i ];
        
        stringstream minerIDStream;
        minerIDStream << i;
        miner.setMinerID ( minerIDStream.str ());
        
        miner.pushGenesisTransaction ( genesisBlock );
    }
    
    genesisBlock.sign ( this->mGenesisKey );
    TheContext::get ().setGenesisBlockDigest ( genesisBlock.getSignature ().getDigest ());
    
    for ( int i = 0; i < nMiners; ++i ) {
        this->mMiners [ i ]->setGenesis ( genesisBlock );
    }
}

//----------------------------------------------------------------//
void Simulation::logMiners ( string prefix ) const {

    int nMiners = this->countMiners ();
    for ( int i = 0; i < nMiners; ++i ) {
        this->mMiners [ i ]->log ( prefix );
    }
}

//----------------------------------------------------------------//
void Simulation::logTree ( string prefix, bool verbose, int maxDepth ) const {

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
void Simulation::resetMinerQueue ( vector < int >& minerQueue, bool shuffle ) {

    int nMiners = this->countMiners ();

    minerQueue.clear ();
    minerQueue.reserve ( nMiners );
    
    for ( int i = 0; i < nMiners; ++i ) {
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
void Simulation::setCyclesPerStep ( int cycles ) {

    this->mCyclesPerStep = cycles;
}

//----------------------------------------------------------------//
void Simulation::setDropRate ( float percentage ) {

    this->mDropRate = percentage;
}

//----------------------------------------------------------------//
void Simulation::setPlayerVerbose ( int minerID, bool verbose ) {

    this->mMiners [ minerID ]->mVerbose = verbose;
}

//----------------------------------------------------------------//
void Simulation::setScoreRandomizer ( bool randomize ) {

    this->mRandomizeScore = randomize;
}

//----------------------------------------------------------------//
Simulation::Simulation () {
}

//----------------------------------------------------------------//
Simulation::~Simulation () {
}

//----------------------------------------------------------------//
void Simulation::step ( size_t step ) {

    int nMiners = this->countMiners ();
    int cycles = this->mCyclesPerStep ? this->mCyclesPerStep : nMiners;

    for ( int i = 0; i < cycles; ++i ) {

        map < SimMiner*, int > schedule;

        for ( int j = 0; j < nMiners; ++j ) {
            SimMiner& miner = *this->mMiners [ j ];
            schedule [ &miner ] = miner.mFrequency;
        }

        while ( schedule.size ()) {

            map < SimMiner*, int >::iterator scheduleIt = next ( schedule.begin (), this->rand () % schedule.size ());

            if ( !this->drop ()) {
                scheduleIt->first->step ();
            }

            scheduleIt->second -= 1;
            if ( scheduleIt->second <= 0 ) {
                schedule.erase ( scheduleIt );
            }
        }
    }
    
    Tree tree;
    for ( int i = 0; i < nMiners; ++i ) {
        tree.addChain ( this->mMiners [ i ]->mChain );
    }
    this->mAnalysis.update ( tree );
    this->Simulation_report ( step );
}

} // namespace Simulator
} // namespace Volition

