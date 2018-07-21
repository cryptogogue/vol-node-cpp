// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/TheContext.h>
#include <volition/simulator/Analysis.h>
#include <volition/simulator/Cohort.h>
#include <volition/simulator/SimMiner.h>
#include <volition/simulator/TheSimulator.h>

namespace Volition {
namespace Simulator {

//================================================================//
// TheSimulator
//================================================================//

//----------------------------------------------------------------//
void TheSimulator::applyCohort ( Cohort& cohort, string name, int baseMinerID, int size ) {

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
int TheSimulator::countMiners () {

    return ( int )this->mMiners.size ();
}

//----------------------------------------------------------------//
bool TheSimulator::drop () {

    return (( float )( rand () % ( 1000 + 1 )) / 1000.0 ) < this->mDropRate;
}

//----------------------------------------------------------------//
const SimMiner& TheSimulator::getMiner ( int minerID ) {

    return *this->mMiners [ minerID ];
}

//----------------------------------------------------------------//
void TheSimulator::initMiners ( int nMiners ) {

    if ( !this->mGenesisKey ) {
        this->mGenesisKey = make_unique < Poco::Crypto::ECKey >( CryptoKey::DEFAULT_EC_GROUP_NAME );
        TheContext::get ().setGenesisBlockKey ( *this->mGenesisKey );
    }

    shared_ptr < Block > genesisBlock = make_shared < Block >();

    this->mMiners.clear ();
    this->mMiners.resize ( nMiners );

    for ( int i = 0; i < nMiners; ++i ) {
        this->mMiners [ i ] = make_unique < SimMiner >();
        SimMiner& miner = *this->mMiners [ i ];
        
        stringstream minerIDStream;
        minerIDStream << i;
        miner.setMinerID ( minerIDStream.str ());
        
        miner.pushGenesis ( *genesisBlock );
    }
    
    genesisBlock->sign ( *this->mGenesisKey );
    TheContext::get ().setGenesisBlockDigest ( genesisBlock->getSignature ().getDigest ());
    
    for ( int i = 0; i < nMiners; ++i ) {
        this->mMiners [ i ]->setGenesis ( genesisBlock );
    }
}

//----------------------------------------------------------------//
void TheSimulator::print () {

    int nMiners = this->countMiners ();
    for ( int i = 0; i < nMiners; ++i ) {
        this->mMiners [ i ]->print ();
    }
}

//----------------------------------------------------------------//
void TheSimulator::printTree ( bool verbose, int maxDepth ) {

    TreeSummary summary;
    this->summarize ( summary );
    
    summary.printLevels ();
    summary.print ( verbose, maxDepth );
}

//----------------------------------------------------------------//
void TheSimulator::process () {

    int nMiners = this->countMiners ();
    int cycles = this->mCyclesPerStep ? this->mCyclesPerStep : nMiners;

    for ( int i = 0; i < cycles; ++i ) {

        map < SimMiner*, int > schedule;

        for ( int j = 0; j < nMiners; ++j ) {
            SimMiner& miner = *this->mMiners [ j ];
            schedule [ &miner ] = miner.mFrequency;
        }

        while ( schedule.size ()) {

            map < SimMiner*, int >::iterator scheduleIt = next ( schedule.begin (), rand () % schedule.size ());

            if ( !this->drop ()) {
                scheduleIt->first->step ();
            }

            scheduleIt->second -= 1;
            if ( scheduleIt->second <= 0 ) {
                schedule.erase ( scheduleIt );
            }
        }
    }
}

//----------------------------------------------------------------//
void TheSimulator::reset () {

    this->mMiners.clear ();
    this->mRand.seed ( 1 );
    
    this->mDropRate           = 0.0;
    this->mCyclesPerStep      = 1;
    this->mRandomizeScore     = false;
}

//----------------------------------------------------------------//
void TheSimulator::resetMinerQueue ( vector < int >& minerQueue, bool shuffle ) {

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
void TheSimulator::setCyclesPerStep ( int cycles ) {

    this->mCyclesPerStep = cycles;
}

//----------------------------------------------------------------//
void TheSimulator::setDropRate ( float percentage ) {

    this->mDropRate = percentage;
}

//----------------------------------------------------------------//
void TheSimulator::setPlayerVerbose ( int minerID, bool verbose ) {

    this->mMiners [ minerID ]->mVerbose = verbose;
}

//----------------------------------------------------------------//
void TheSimulator::setScoreRandomizer ( bool randomize ) {

    this->mRandomizeScore = randomize;
}

//----------------------------------------------------------------//
void TheSimulator::summarize ( TreeSummary& summary ) {

    Tree tree;
    int nMiners = this->countMiners ();
    for ( int i = 0; i < nMiners; ++i ) {
        tree.addChain ( *this->mMiners [ i ]->mChain );
    }
    summary.summarize ( tree );
}

} // namespace Simulator
} // namespace Volition

