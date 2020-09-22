// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

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
void Simulator::initialize ( size_t totalMiners, int basePort ) {

    this->mMiners.resize ( totalMiners );

    shared_ptr < Transactions::Genesis > genesisMinerTransactionBody = make_unique < Transactions::Genesis >();
    genesisMinerTransactionBody->setIdentity ( "SIMULATION" );

    this->mMessenger = make_shared < SimMiningMessenger >();

    for ( size_t i = 0; i < totalMiners; ++i ) {
    
        shared_ptr < Miner > miner = make_shared < Miner >();
        this->mMiners [ i ] = miner;

        miner->setMinerID ( Format::write ( "%d", basePort + ( int )i ));
        miner->affirmKey ();
        miner->affirmVisage ();
        miner->setMessenger ( this->mMessenger );

        Transactions::GenesisAccount genesisAccount;
        
        genesisAccount.mName    = miner->getMinerID ();
        genesisAccount.mKey     = miner->getKeyPair ();
        genesisAccount.mGrant   = 0;
        genesisAccount.mURL     = Format::write ( "http://127.0.0.1:%s/%s/", Format::write ( "%d", basePort ).c_str (), miner->getMinerID ().c_str ());

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
Simulator::Simulator () {
}

//----------------------------------------------------------------//
Simulator::~Simulator () {
}

//----------------------------------------------------------------//
void Simulator::step () {
        
    Simulation::Tree tree;
    
    for ( size_t i = 0; i < this->mMiners.size (); ++i ) {
        shared_ptr < Miner > miner = this->mMiners [ i ];
        miner->step ();
        ScopedMinerLock minerLock ( miner );
        tree.addChain ( *this->mMiners [ i ]->getBestBranch ());
    }
    
    this->mMessenger->updateAndDispatch ();
    
//    shared_ptr < const BlockTreeNode > tail = this->mMiners [ 0 ]->getBlockTreeTag ();
//    LGN_LOG ( VOL_FILTER_ROOT, INFO, "9090: %s", tail->writeBranch ().c_str ());
//
//    this->extendOptimal (( **tail ).getHeight ());
//    LGN_LOG ( VOL_FILTER_ROOT, INFO, "GOAL: %s", this->mOptimalTag->writeBranch ().c_str ());
//
//    LGN_LOG ( VOL_FILTER_ROOT, INFO, "" );
    
    this->mAnalysis.update ( tree );
    this->mAnalysis.log ( "", true, 1 );
}

} // namespace Simulator
} // namespace Volition
