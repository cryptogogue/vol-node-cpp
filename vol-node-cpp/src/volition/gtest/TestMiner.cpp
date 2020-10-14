// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/Block.h>
#include <volition/CryptoKey.h>
#include <volition/Miner.h>
#include <volition/Transactions.h>

using namespace Volition;

//================================================================//
// SimpleMiner
//================================================================//
class SimpleMiner :
    public Miner{
public:
    
    //----------------------------------------------------------------//
    bool checkBranch ( string miners ) {
        
        this->selectBranch ();
        const Ledger* chain = this->getBestBranch ();
        assert ( chain );
        return chain->checkMiners ( miners );
    }
    
    //----------------------------------------------------------------//
    void pushGenesisTransaction ( Block& block ) {
        
        unique_ptr < Transactions::GenesisMiner > genesisMinerTransaction = make_unique < Transactions::GenesisMiner >();
        
        genesisMinerTransaction->mAccountName   = this->mMinerID;
        genesisMinerTransaction->mKey           = this->mKeyPair;
        genesisMinerTransaction->mKeyName       = "master";
        genesisMinerTransaction->mAmount        = 0;
        genesisMinerTransaction->mURL           = "";

        block.pushTransaction ( move ( genesisMinerTransaction ));
    }
    
    //----------------------------------------------------------------//
    SimpleMiner () {
    }
    
    //----------------------------------------------------------------//
    SimpleMiner ( string minerID, u64 now ) {
        this->setMinerID ( minerID );
        this->mKeyPair.elliptic ();
        this->setTime ( now );
    }
};

//----------------------------------------------------------------//
void initializeTestChainAndMiners ( SimpleMiner* miners, size_t nMiners, u64 now ) {

    Block genesisBlock;

    for ( size_t i = 0; i < nMiners; ++i ) {
    
        stringstream minerIDStream;
        minerIDStream << ( int )i;
    
        SimpleMiner& miner = miners [ i ];
        miner = SimpleMiner ( minerIDStream.str (), now );
        
        miner.pushGenesisTransaction ( genesisBlock );
    }
    
    CryptoPrivateKey genesisKey;
    genesisKey.elliptic ();
    TheContext::get ().setGenesisBlockKey ( genesisKey );
    
    genesisBlock.sign ( genesisKey );
    TheContext::get ().setGenesisBlockDigest ( genesisBlock.getSignature ().getDigest ());
    
    for ( size_t i = 0; i < nMiners; ++i ) {
        SimpleMiner& miner = miners [ i ];
        miner.setGenesis ( genesisBlock );
    }
}

//----------------------------------------------------------------//
TEST ( Ledger, test0 ) {

    TheContext::get ().setScoringMode ( TheContext::ScoringMode::INTEGER, 2 );
    TheContext::get ().setWindow ( 1000 );

    SimpleMiner miners [ 2 ];
    initializeTestChainAndMiners ( miners, 2, 10000 );

    ASSERT_TRUE ( miners [ 0 ].countBranches () == 1 );
    ASSERT_TRUE ( miners [ 0 ].getLongestBranchSize () == 1 );

    ASSERT_TRUE ( miners [ 1 ].countBranches () == 1 );
    ASSERT_TRUE ( miners [ 1 ].getLongestBranchSize () == 1 );

    miners [ 0 ].extend ();
    ASSERT_TRUE ( miners [ 0 ].checkBranch ( "-,0" ));
    
    miners [ 1 ].extend ();
    ASSERT_TRUE ( miners [ 1 ].checkBranch ( "-,1" ));
    
    miners [ 0 ].submitChain ( *miners [ 1 ].getBestBranch ());
    ASSERT_TRUE ( miners [ 0 ].countBranches () == 2 );
    ASSERT_TRUE ( miners [ 0 ].hasBranch ( "-,0" ));
    ASSERT_TRUE ( miners [ 0 ].hasBranch ( "-,1" ));
    ASSERT_TRUE ( miners [ 0 ].checkBranch ( "-,0" ));
    
    miners [ 1 ].submitChain ( *miners [ 0 ].getBestBranch ());
    ASSERT_TRUE ( miners [ 1 ].countBranches () == 2 );
    ASSERT_TRUE ( miners [ 1 ].hasBranch ( "-,0" ));
    ASSERT_TRUE ( miners [ 1 ].hasBranch ( "-,1" ));
    ASSERT_TRUE ( miners [ 1 ].checkBranch ( "-,0" ));
    
    miners [ 0 ].extend ();
    ASSERT_TRUE ( miners [ 0 ].countBranches () == 2 );
    ASSERT_TRUE ( miners [ 0 ].hasBranch ( "-,0" ));
    ASSERT_TRUE ( miners [ 0 ].hasBranch ( "-,0,0" ));
    ASSERT_TRUE ( miners [ 0 ].checkBranch ( "-,0,0" ));
    
    miners [ 1 ].extend ();
    ASSERT_TRUE ( miners [ 1 ].countBranches () == 2 );
    ASSERT_TRUE ( miners [ 1 ].hasBranch ( "-,0,1" ));
    ASSERT_TRUE ( miners [ 1 ].hasBranch ( "-,1,1" ));
    ASSERT_TRUE ( miners [ 1 ].checkBranch ( "-,0,1" ));
    
    miners [ 0 ].submitChain ( *miners [ 1 ].getBestBranch ());
    ASSERT_TRUE ( miners [ 0 ].checkBranch ( "-,0,1" ));
    
    miners [ 1 ].submitChain ( *miners [ 0 ].getBestBranch ());
    ASSERT_TRUE ( miners [ 1 ].checkBranch ( "-,0,1" ));
    
    miners [ 0 ].extend ();
    ASSERT_TRUE ( miners [ 0 ].checkBranch ( "-,0,1,0" ));
    
    miners [ 1 ].extend ();
    ASSERT_TRUE ( miners [ 1 ].checkBranch ( "-,0,1,1" ));
    
    miners [ 0 ].submitChain ( *miners [ 1 ].getBestBranch ());
    ASSERT_TRUE ( miners [ 0 ].checkBranch ( "-,0,1,0" ));
    
    miners [ 1 ].submitChain ( *miners [ 0 ].getBestBranch ());
    ASSERT_TRUE ( miners [ 1 ].checkBranch ( "-,0,1,0" ));
}

//----------------------------------------------------------------//
TEST ( Ledger, test1 ) {

//    TheContext::get ().setScoringMode ( TheContext::ScoringMode::INTEGER );
//
//    Ledger chain;
//    SimpleMiner miners [ 4 ];
//    initializeTestChainAndMiners ( chain, miners, 4 );
//
//    // cycle 1
//
//    // 1.0 in reverse order
//    miners [ 3 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 2 );
//    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 1 );
//
//    miners [ 2 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 2 );
//    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 1 );
//
//    miners [ 1 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 2 );
//    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 1 );
//
//    miners [ 0 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 2 );
//    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 1 );
//
//    // 1.1 in reverse order
//    miners [ 3 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 2 );
//    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 2 );
//
//    miners [ 2 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 2 );
//    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 2 );
//
//    miners [ 1 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 2 );
//    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 2 );
//
//    // 1.2 in reverse order
//    miners [ 3 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 2 );
//    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 3 );
//
//    miners [ 2 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 2 );
//    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 3 );
//
//    // 1.3 in reverse order
//    miners [ 3 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 2 );
//    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 4 );
//
//    // cycle 2
//
//    // 2.0 in reverse order
//    miners [ 3 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 3 );
//    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 1 );
//
//    miners [ 2 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 3 );
//    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 1 );
//
//    miners [ 1 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 3 );
//    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 1 );
//
//    miners [ 0 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 3 );
//    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 1 );
//
//    // 2.1 in reverse order
//    miners [ 3 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 3 );
//    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 2 );
//
//    miners [ 2 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 3 );
//    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 2 );
//
//    miners [ 1 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 3 );
//    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 2 );
//
//    // 2.2 in reverse order
//    miners [ 3 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 3 );
//    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 3 );
//
//    miners [ 2 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 3 );
//    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 3 );
//
//    // 2.3 in reverse order
//    miners [ 3 ].pushBlock ( chain );
//    ASSERT_TRUE ( chain.countCycles () == 3 );
//    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 4 );
//
//    ASSERT_TRUE ( chain.countCycles () == 3 );
//    ASSERT_TRUE ( chain.countBlocks ( 0 ) == 1 ); // genesis cycle
//    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 4 );
//    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 4 );
}

//----------------------------------------------------------------//
TEST ( Ledger, test3 ) {

//    TheContext::get ().setScoringMode ( TheContext::ScoringMode::INTEGER );
//
//    stringstream strStream;
//
//    {
//        Ledger chain;
//        SimpleMiner miners [ 3 ];
//        initializeTestChainAndMiners ( chain, miners, 3 );
//        
//        miners [ 0 ].pushBlock ( chain );
//        miners [ 1 ].pushBlock ( chain );
//        miners [ 2 ].pushBlock ( chain );
//        
//        miners [ 0 ].pushBlock ( chain );
//        miners [ 1 ].pushBlock ( chain );
//        miners [ 2 ].pushBlock ( chain );
//        
//        ASSERT_TRUE ( chain.countCycles () == 3 );
//        ASSERT_TRUE ( chain.countBlocks ( 0 ) == 1 ); // genesis cycle
//        ASSERT_TRUE ( chain.countBlocks ( 1 ) == 3 );
//        ASSERT_TRUE ( chain.countBlocks ( 2 ) == 3 );
//
//        ASSERT_TRUE ( chain.print () == "[.][0,1,2 (3)][0,1,2 (3)]" );
//
//        ToJSONSerializer::toJSON ( chain, strStream );
//    }
//    
//    Ledger chain;
//    FromJSONSerializer::fromJSON ( chain, strStream );
// 
//    ASSERT_TRUE ( chain.countCycles () == 3 );
//    ASSERT_TRUE ( chain.countBlocks ( 0 ) == 1 ); // genesis cycle
//    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 3 );
//    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 3 );
//    
//    ASSERT_TRUE ( chain.print () == "[.][0,1,2 (3)][0,1,2 (3)]" );
}
