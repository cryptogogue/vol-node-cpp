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
    SimpleMiner ( string minerID ) :
        mMinerID ( minerID ) {
        this->mKeyPair.elliptic ();
    }
};

//----------------------------------------------------------------//
void pushBlock ( Chain& chain, const SimpleMiner& miner ) {
    
    ASSERT_TRUE ( chain.canPush ( miner.mMinerID, true ));
    
    ChainPlacement placement = chain.findNextCycle ( miner.mMinerID );
    Block block ( miner.mMinerID, placement.getCycleID (), miner.mKeyPair );
    chain.prepareForPush ( placement, block );

    bool result = chain.pushBlockAndSign ( block, miner.mKeyPair );
    ASSERT_TRUE ( result );
}

//----------------------------------------------------------------//
void pushMinerGenesisTransaction ( Block& block, const SimpleMiner& miner  ) {

    unique_ptr < Transaction::GenesisMiner > genesisMinerTransaction = make_unique < Transaction::GenesisMiner >();
    
    genesisMinerTransaction->mAccountName   = miner.mMinerID;
    genesisMinerTransaction->mKey           = miner.mKeyPair;
    genesisMinerTransaction->mKeyName       = "master";
    genesisMinerTransaction->mAmount        = 0;
    genesisMinerTransaction->mURL           = "";

    block.pushTransaction ( move ( genesisMinerTransaction ));
}

//----------------------------------------------------------------//
TEST ( Chain, unitTests ) {

    TheContext::get ().setScoringMode ( TheContext::ScoringMode::INTEGER );

    SimpleMiner miner0 ( "0" );
    SimpleMiner miner1 ( "1" );

    Block genesisBlock;

    pushMinerGenesisTransaction ( genesisBlock, miner0 );
    pushMinerGenesisTransaction ( genesisBlock, miner1 );

    CryptoKey genesisKey;
    genesisKey.elliptic ();
    TheContext::get ().setGenesisBlockKey ( genesisKey );
    
    genesisBlock.sign ( genesisKey );
    TheContext::get ().setGenesisBlockDigest ( genesisBlock.getSignature ().getDigest ());
    
    Chain chain ( genesisBlock );
    
    ASSERT_TRUE ( chain.countCycles () == 1 );
    ASSERT_TRUE ( chain.countBlocks ( 0 ) == 1 );
    ASSERT_TRUE ( chain.countBlocks () == 1 );
    
    // cycle 0
    pushBlock ( chain, miner0 );
    pushBlock ( chain, miner1 );
    
    // cycle 1
    pushBlock ( chain, miner0 );
    pushBlock ( chain, miner1 );
    
    // cycle 2
    pushBlock ( chain, miner0 );
    pushBlock ( chain, miner1 );
   
    ASSERT_TRUE ( chain.countCycles () == 4 );
    ASSERT_TRUE ( chain.countBlocks ( 0 ) == 1 ); // genesis cycle
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 3 ) == 2 );
}
