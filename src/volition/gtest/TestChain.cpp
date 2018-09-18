// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/Block.h>
#include <volition/Chain.h>
#include <volition/CryptoKey.h>
#include <volition/TheContext.h>
#include <volition/Transactions.h>
#include <volition/VersionedStore.h>

using namespace Volition;

//================================================================//
// SimpleMiner
//================================================================//
class SimpleMiner {
public:

    string      mMinerID;
    CryptoKey   mKeyPair;

  
    //----------------------------------------------------------------//
    void pushBlock ( Chain& chain ) {
    
        ASSERT_TRUE ( chain.canPush ( this->mMinerID, true ));
    
        ChainPlacement placement = chain.findNextCycle ( this->mMinerID );
        Block block ( this->mMinerID, placement.getCycleID (), this->mKeyPair );
        chain.prepareForPush ( placement, block );

        bool result = chain.pushBlockAndSign ( block, this->mKeyPair );
        ASSERT_TRUE ( result );
    }
    
    //----------------------------------------------------------------//
    void pushMinerGenesisTransaction ( Block& block ) {
        
        unique_ptr < Transaction::GenesisMiner > genesisMinerTransaction = make_unique < Transaction::GenesisMiner >();
        
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
    SimpleMiner ( string minerID ) :
        mMinerID ( minerID ) {
        this->mKeyPair.elliptic ();
    }
};

//----------------------------------------------------------------//
Chain initializeTestChainAndMiners ( SimpleMiner* miners, size_t nMiners ) {

    Block genesisBlock;

    for ( size_t i = 0; i < nMiners; ++i ) {
    
        stringstream minerIDStream;
        minerIDStream << ( int )i;
    
        SimpleMiner& miner = miners [ i ];
        miner = SimpleMiner ( minerIDStream.str ());
        
        miner.pushMinerGenesisTransaction ( genesisBlock );
    }
    
    CryptoKey genesisKey;
    genesisKey.elliptic ();
    TheContext::get ().setGenesisBlockKey ( genesisKey );
    
    genesisBlock.sign ( genesisKey );
    TheContext::get ().setGenesisBlockDigest ( genesisBlock.getSignature ().getDigest ());
    
    return Chain ( genesisBlock );
}

//----------------------------------------------------------------//
TEST ( Chain, test0 ) {

    TheContext::get ().setScoringMode ( TheContext::ScoringMode::INTEGER );

    SimpleMiner miners [ 2 ];
    Chain chain = initializeTestChainAndMiners ( miners, 2 );
    
    ASSERT_TRUE ( chain.countCycles () == 1 );
    ASSERT_TRUE ( chain.countBlocks ( 0 ) == 1 );
    ASSERT_TRUE ( chain.countBlocks () == 1 );
    
    // cycle 0
    miners [ 0 ].pushBlock ( chain );
    miners [ 1 ].pushBlock ( chain );
    
    // cycle 1
    miners [ 0 ].pushBlock ( chain );
    miners [ 1 ].pushBlock ( chain );
    
    // cycle 2
    miners [ 0 ].pushBlock ( chain );
    miners [ 1 ].pushBlock ( chain );
   
    ASSERT_TRUE ( chain.countCycles () == 4 );
    ASSERT_TRUE ( chain.countBlocks ( 0 ) == 1 ); // genesis cycle
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 3 ) == 2 );
}
