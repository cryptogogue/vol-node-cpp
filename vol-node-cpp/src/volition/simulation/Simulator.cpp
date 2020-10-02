// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/simulation/AbstractScenario.h>
#include <volition/simulation/Analysis.h>
#include <volition/simulation/Simulator.h>
#include <volition/transactions/Genesis.h>

namespace Volition {
namespace Simulation {

//================================================================//
// Simulator
//================================================================//

//----------------------------------------------------------------//
void Simulator::extendOptimal ( size_t height ) {

    BlockTreeNode::ConstPtr tail = this->mOptimalTag;
    assert ( tail );
    
    while (( **tail ).getHeight () < height ) {
                
        shared_ptr < const Block > parent = tail->getBlock ();
        
        shared_ptr < Miner > bestMiner;
        Digest bestCharm;
        
        for ( size_t i = 0; i < this->mMiners.size (); ++i ) {
            shared_ptr < Miner > miner = this->mMiners [ i ];
            Digest charm = parent->getNextCharm ( miner->getVisage ());
                            
            if ( !bestMiner || ( BlockHeader::compare ( charm, bestCharm ) < 0 )) {
                bestMiner = miner;
                bestCharm = charm;
            }
        }
        
        shared_ptr < Block > child = make_shared < Block >(
            bestMiner->getMinerID (),
            bestMiner->getVisage (),
            0,
            parent.get (),
            bestMiner->getKeyPair ()
        );
        child->sign ( bestMiner->getKeyPair ());

        tail = this->mOptimal.affirmBlock ( child );
        this->mOptimalTag = tail;
    }
}

//----------------------------------------------------------------//
const Simulator::Miners& Simulator::getMiners () {

    return this->mMiners;
}

//----------------------------------------------------------------//
void Simulator::initialize ( size_t totalMiners, size_t basePort ) {

    this->mMiners.resize ( totalMiners );
    this->mMinerSettings.resize ( totalMiners );

    shared_ptr < Transactions::Genesis > genesisMinerTransactionBody = make_unique < Transactions::Genesis >();
    genesisMinerTransactionBody->setIdentity ( "SIMULATION" );

    this->mMessenger = make_shared < SimMiningMessenger >();

    for ( size_t i = 0; i < totalMiners; ++i ) {
    
        shared_ptr < Miner > miner = make_shared < Miner >();
        this->mMiners [ i ] = miner;

        miner->setMinerID ( Format::write ( "%d", ( int )( basePort + i )));
        miner->affirmKey ();
        miner->affirmVisage ();
        miner->setMessenger ( this->mMessenger );

        Transactions::GenesisAccount genesisAccount;
        
        genesisAccount.mName    = miner->getMinerID ();
        genesisAccount.mKey     = miner->getKeyPair ();
        genesisAccount.mGrant   = 0;
        genesisAccount.mURL     = Format::write ( "http://127.0.0.1:%s/%s/", Format::write ( "%d", ( int )basePort ).c_str (), miner->getMinerID ().c_str ());

        genesisAccount.mMotto   = miner->getMotto ();
        genesisAccount.mVisage  = miner->getVisage ();

        genesisMinerTransactionBody->pushAccount ( genesisAccount );
    }
    
    this->mMessenger->setMiners ( this->mMiners );

    shared_ptr < Transaction > transaction = make_shared < Transaction >();
    transaction->setBody ( move ( genesisMinerTransactionBody ));
    
    shared_ptr < Block > genesisBlock = make_shared < Block >();
    genesisBlock->pushTransaction ( transaction );
    genesisBlock->affirmHash ();

    for ( size_t i = 0; i < totalMiners; ++i ) {
        this->mMiners [ i ]->setGenesis ( genesisBlock );
    }

    this->mOptimalTag = this->mOptimal.affirmBlock ( genesisBlock );
}

//----------------------------------------------------------------//
void Simulator::initialize ( shared_ptr < AbstractScenario > scenario ) {

    assert ( scenario );

    this->mScenario = scenario;
    this->initialize (
        scenario->AbstractScenario_getSize (),
        scenario->AbstractScenario_getBasePort ()
    );
    this->setReportMode ( scenario->AbstractScenario_getReportMode ());
    scenario->AbstractScenario_setup ( *this );
}

//----------------------------------------------------------------//
void Simulator::pause ( bool pause ) {

    this->mIsPaused = pause;
}

//----------------------------------------------------------------//
void Simulator::report () {

    switch ( this->mReportMode ) {
    
        case REPORT_SUMMARY: {
        
            this->mAnalysis.log ( "", false, 1 );
        
            break;
        }
            
        case REPORT_SINGLE_MINER: {
        
            shared_ptr < Miner > miner = this->mMiners [ 0 ];
            BlockTreeNode::ConstPtr tail = miner->getBestBranch ();
                
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s: %s", miner->getMinerID ().c_str (), tail->writeBranch ().c_str ());
            
            break;
        }
            
        case REPORT_SINGLE_MINER_VS_OPTIMAL: {
            
            shared_ptr < Miner > miner = this->mMiners [ 0 ];
            BlockTreeNode::ConstPtr tail = miner->getBestBranch ();
                
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s: %s", miner->getMinerID ().c_str (), tail->writeBranch ().c_str ());
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "GOAL: %s", this->mOptimalTag->writeBranch ().c_str ());
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "" );
        
            break;
        }
            
        case REPORT_ALL_MINERS: {
        
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "STEP: %d", ( int )this->mStepCount );
            for ( size_t i = 0; i < this->mMiners.size (); ++i ) {
                shared_ptr < Miner > miner = this->mMiners [ i ];
                BlockTreeNode::ConstPtr tail = miner->getBestBranch ();
                LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s: %s", miner->getMinerID ().c_str (), tail->writeBranch ().c_str ());
            }
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "" );
        
            break;
        }
    }
}

//----------------------------------------------------------------//
void Simulator::setInterval ( size_t base, size_t top, size_t interval ) {

    for ( size_t i = base; i < top; ++i ) {
        this->mMinerSettings [ i ].mInterval = interval;
    }
}


//----------------------------------------------------------------//
void Simulator::setReportMode ( ReportMode reportMode ) {

    this->mReportMode = reportMode;
}

//----------------------------------------------------------------//
void Simulator::setTimeStep ( time_t seconds ) {

    this->mTimeStep = seconds;
}

//----------------------------------------------------------------//
Simulator::Simulator () :
    mReportMode ( REPORT_SUMMARY ),
    mIsPaused ( false ),
    mStepCount ( 0 ),
    mTimeStep ( 0 ) {
}

//----------------------------------------------------------------//
Simulator::~Simulator () {
}

//----------------------------------------------------------------//
void Simulator::step () {

    if ( this->mScenario ) {
        this->mScenario->AbstractScenario_control ( *this, this->mStepCount );
    }

    if ( this->mIsPaused ) return;

    if (( this->mTimeStep == 0 ) || ( this->mStepCount == 0 )) {
        time ( &this->mNow );
    }
    else {
        this->mNow += this->mTimeStep;
    }

    Simulation::Tree tree;
    
    for ( size_t i = 0; i < this->mMiners.size (); ++i ) {
        
        shared_ptr < Miner > miner = this->mMiners [ i ];
        SimMinerSettings& settings = this->mMinerSettings [ i ];
        
        if ( settings.mInterval && (( this->mStepCount % settings.mInterval ) == 0 )) {
            miner->step ( this->mNow );
        }
        
        ScopedMinerLock minerLock ( miner );
        tree.addChain ( *this->mMiners [ i ]->getChain ());
    }
    
    this->mMessenger->updateAndDispatch ();
    
    this->mAnalysis.update ( tree );
    
    BlockTreeNode::ConstPtr tail = this->mMiners [ 0 ]->getBestBranch ();
    this->extendOptimal (( **tail ).getHeight ());
    
    this->report ();
    
    this->mStepCount++;
}

} // namespace Simulator
} // namespace Volition
