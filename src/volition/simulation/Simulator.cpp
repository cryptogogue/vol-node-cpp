// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/FileSys.h>
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

    BlockTreeCursor tail = *this->mOptimalTag;
    assert ( tail.hasHeader ());
    
    while ( tail.getHeight () < height ) {
                
        shared_ptr < const Block > parent = tail.getBlock ();
        
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
        
        shared_ptr < Block > child = make_shared < Block >();
        child->initialize (
            bestMiner->getMinerID (),
            bestMiner->getVisage (),
            0,
            parent.get (),
            bestMiner->getKeyPair ()
        );
        child->sign ( bestMiner->getKeyPair ());

        tail = this->mOptimal.affirmBlock ( this->mOptimalTag, child );
    }
}

//----------------------------------------------------------------//
//SimMiningMessenger& Simulator::getMessenger () {
//
//    assert ( this->mMessenger );
//    return *this->mMessenger;
//}

//----------------------------------------------------------------//
const Simulator::Miners& Simulator::getMiners () {

    return this->mMiners;
}

//----------------------------------------------------------------//
shared_ptr < SimMiner > Simulator::getSimMiner ( size_t idx ) {

    return dynamic_pointer_cast < SimMiner >( this->mMiners [ idx ]);
}

//----------------------------------------------------------------//
void Simulator::initialize ( shared_ptr < AbstractScenario > scenario ) {

    assert ( scenario );

    this->setReportMode ( REPORT_SUMMARY );
    this->mScenario = scenario;
    scenario->AbstractScenario_setup ( *this );
}

//----------------------------------------------------------------//
void Simulator::initializeGenesis ( time_t blockDelayInSeconds, time_t rewriteWindowInSeconds, size_t maxBlockWeight  ) {

    size_t totalMiners = this->mMiners.size ();
    assert ( totalMiners > 0 );

    SerializableVector < CryptoKeyPair > minerKeys;
    SerializableVector < CryptoKeyPair > keyDump;

    static const cc8* MINER_KEYS_FILENAME = "sim-miner-keys.json";
    if ( FileSys::exists ( MINER_KEYS_FILENAME )) {
        FromJSONSerializer::fromJSONFile ( minerKeys, MINER_KEYS_FILENAME );
    }

    shared_ptr < Transactions::Genesis > genesisMinerTransactionBody = make_unique < Transactions::Genesis >();
    
    genesisMinerTransactionBody->setIdentity ( "SIMULATION" );
    genesisMinerTransactionBody->setBlockDelayInSeconds ( blockDelayInSeconds );
    genesisMinerTransactionBody->setRewriteWindowInSeconds ( rewriteWindowInSeconds );
    genesisMinerTransactionBody->setMaxBlockWeight ( maxBlockWeight );

    for ( size_t i = 0; i < totalMiners; ++i ) {
    
        shared_ptr < SimMiner > miner = dynamic_pointer_cast < SimMiner >( this->mMiners [ i ]);
        assert ( miner );

        if ( i < minerKeys.size ()) {
            miner->setKeyPair ( minerKeys [ i ]);
        }
        miner->affirmKey ();
        miner->affirmVisage ();

        keyDump.push_back ( miner->getKeyPair ());

        Transactions::GenesisAccount genesisAccount;
        
        genesisAccount.mName            = miner->getMinerID ();
        genesisAccount.mKey             = miner->getKeyPair ().getPublicKey ();
        genesisAccount.mGrant           = 0;
        
        if ( miner->mIsGenesisMiner ) {
            
            shared_ptr < const MinerInfo > minerInfo = make_shared < MinerInfo >(
                miner->getURL (),
                miner->getKeyPair ().getPublicKey (),
                miner->getMotto (),
                miner->getVisage ()
            );
            genesisAccount.mMinerInfo = minerInfo;
        }
        genesisMinerTransactionBody->pushAccount ( genesisAccount );
    }
    
    LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s", ToJSONSerializer::toJSONString ( keyDump, 4 ).c_str ());
    
    shared_ptr < Transaction > transaction = make_shared < Transaction >();
    transaction->setBody ( move ( genesisMinerTransactionBody ));
    
    shared_ptr < Block > genesisBlock = make_shared < Block >();
    genesisBlock->setBlockDelayInSeconds ( 1 );
    genesisBlock->setRewriteWindow ( 10 );
    genesisBlock->pushTransaction ( transaction );
    genesisBlock->affirmHash ();

    for ( size_t i = 0; i < totalMiners; ++i ) {
        this->mMiners [ i ]->setGenesis ( genesisBlock );
    }
    this->mOptimal.affirmBlock ( this->mOptimalTag, genesisBlock );
}

//----------------------------------------------------------------//
void Simulator::initializeMiners ( size_t totalMiners, size_t deferredMiners, size_t basePort ) {

    this->mBasePort = basePort;

    this->mMiners.resize ( totalMiners );
    this->mNetwork = make_shared < SimMiningNetwork >();

    size_t genesisMiners = totalMiners - deferredMiners;

    for ( size_t i = 0; i < totalMiners; ++i ) {
    
        shared_ptr < SimMiner > miner = make_shared < SimMiner >( i < genesisMiners );
        this->mMiners [ i ] = miner;

        miner->setMinerID ( Format::write ( "%d", ( int )( basePort + i )));
        miner->setURL ( Format::write ( "http://127.0.0.1:%d/%s/", ( int )this->mBasePort, miner->getMinerID ().c_str ()));
        miner->setMessenger ( make_shared < SimMiningMessenger >( this->mNetwork ));
    }
    
    this->mNetwork->setMiners ( this->mMiners );
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
            BlockTreeCursor tail = miner->getBestProvisional ();
            
            miner->report ( Miner::REPORT_ALL_BRANCHES );
                        
            break;
        }
            
        case REPORT_SINGLE_MINER_VS_OPTIMAL: {
            
            shared_ptr < Miner > miner = this->mMiners [ 0 ];
            BlockTreeCursor tail = miner->getBestProvisional ();
                
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s: %s", miner->getMinerID ().c_str (), tail.writeBranch ().c_str ());
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "GOAL: %s", ( *this->mOptimalTag ).writeBranch ().c_str ());
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "" );
        
            break;
        }
            
        case REPORT_ALL_MINERS: {
        
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "STEP: %d", ( int )this->mStepCount );
            for ( size_t i = 0; i < this->mMiners.size (); ++i ) {
            
                shared_ptr < Miner > miner = this->mMiners [ i ];
                BlockTreeCursor tail = miner->getBestProvisional ();
                
                LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s: %s", miner->getMinerID ().c_str (), tail.writeBranch ().c_str ());
            }
            
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "" );
        
            break;
        }
        
        case REPORT_ALL_MINERS_VERBOSE: {
        
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "----------------------------------------------------------------" );
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "STEP: %d", ( int )this->mStepCount );
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "" );
            
            for ( size_t i = 0; i < this->mMiners.size (); ++i ) {
            
                shared_ptr < Miner > miner = this->mMiners [ i ];
                BlockTreeCursor tail = miner->getBestProvisional ();
                
                miner->report ( Miner::REPORT_ALL_BRANCHES );
            }
            break;
        }
    }
}

//----------------------------------------------------------------//
void Simulator::setActive ( size_t base, size_t top, bool active ) {

    for ( size_t i = base; i < top; ++i ) {
        shared_ptr < SimMiner > miner = dynamic_pointer_cast < SimMiner >( this->mMiners [ i ]);
        assert ( miner );
        miner->setActive ( active );
    }
}

//----------------------------------------------------------------//
void Simulator::setInterval ( size_t base, size_t top, size_t interval ) {

    for ( size_t i = base; i < top; ++i ) {
        shared_ptr < SimMiner > miner = dynamic_pointer_cast < SimMiner >( this->mMiners [ i ]);
        assert ( miner );
        miner->mInterval = interval;
    }
}

//----------------------------------------------------------------//
void Simulator::setMinerKey ( size_t idx, const CryptoKeyPair& key ) {

    this->mMiners [ idx ]->setKeyPair ( key );
}

//----------------------------------------------------------------//
void Simulator::setMinerKey ( size_t idx, string pem ) {

    CryptoKeyPair key;
    key.rsaFromPEM ( "", pem );
    this->setMinerKey ( idx, key );
}

//----------------------------------------------------------------//
void Simulator::setReportMode ( ReportMode reportMode ) {

    this->mReportMode = reportMode;
}

//----------------------------------------------------------------//
void Simulator::setTimeStepInSeconds ( time_t seconds ) {

    this->mTimeStep = seconds;
}

//----------------------------------------------------------------//
Simulator::Simulator () :
    mReportMode ( REPORT_SUMMARY ),
    mBasePort ( 0 ),
    mIsPaused ( false ),
    mStepCount ( 0 ),
    mTimeStep ( 1 ) {
    
    this->mOptimalTag.setName ( "optimal" );
}

//----------------------------------------------------------------//
Simulator::~Simulator () {
}

//----------------------------------------------------------------//
void Simulator::step () {

    if ( this->mIsPaused ) return;

    if ( this->mScenario ) {
        this->mScenario->AbstractScenario_control ( *this, *this->mNetwork, this->mStepCount );
    }

    if (( this->mTimeStep == 0 ) || ( this->mStepCount == 0 )) {
        time ( &this->mNow );
    }
    else {
        this->mNow += this->mTimeStep;
    }

    Simulation::Tree tree;
    
    for ( size_t i = 0; i < this->mMiners.size (); ++i ) {
        
        shared_ptr < SimMiner > miner = dynamic_pointer_cast < SimMiner >( this->mMiners [ i ]);
        assert ( miner );
        
        if ( miner->mActive && ( miner->mInterval && (( this->mStepCount % miner->mInterval ) == 0 ))) {
            miner->step ( this->mNow );
        }
        
        ScopedMinerLock minerLock ( miner );
        tree.addChain ( this->mMiners [ i ]->getLedger ());
    }
    
    this->mNetwork->updateAndDispatch ();
    
    this->mAnalysis.update ( tree );
    
    BlockTreeCursor tail = this->mMiners [ 0 ]->getBestProvisional ();
    this->extendOptimal ( tail.getHeight ());
    
    this->report ();
    
    this->mStepCount++;
}

} // namespace Simulator
} // namespace Volition
