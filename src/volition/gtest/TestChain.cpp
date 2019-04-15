// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/Block.h>
#include <volition/Chain.h>
#include <volition/ChainMetadata.h>
#include <volition/CryptoKey.h>
#include <volition/TheContext.h>
#include <volition/Transactions.h>

using namespace Volition;

//================================================================//
// SimpleMiner
//================================================================//
class SimpleMiner {
public:

    string          mMinerID;
    CryptoKey       mKeyPair;
    ChainMetadata   mMetaData;
  
    //----------------------------------------------------------------//
    void pushBlock ( Chain& chain ) {
        
        VersionedStoreIterator chainIt ( chain, 0 );
        for ( ; chainIt && ( !chainIt.isCurrent ()); chainIt.next ()) {
            shared_ptr < Block > block = Ledger::getJSONSerializableObject < Block >( chainIt, Chain::BLOCK_KEY );
            assert ( block );
            this->mMetaData.affirmParticipant ( block->getCycleID (), block->getMinerID ());
        }
        
        ASSERT_TRUE ( chain.canPush ( this->mMinerID, true ));
        
        ChainPlacement placement = chain.findNextCycle ( this->mMetaData, this->mMinerID );
        Block block ( this->mMinerID, placement.getCycleID (), this->mKeyPair );
        chain.prepareForPush ( this->mMetaData, placement, block );
        block.sign ( this->mKeyPair );
        
        bool result = chain.pushBlock ( block );
        ASSERT_TRUE ( result );
        
        this->mMetaData.affirmParticipant ( block.getCycleID (), block.getMinerID ());
    }
    
    //----------------------------------------------------------------//
    void pushMinerGenesisTransaction ( Block& block ) {
        
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
    SimpleMiner ( string minerID ) :
        mMinerID ( minerID ) {
        this->mKeyPair.elliptic ();
    }
};

//----------------------------------------------------------------//
void initializeTestChainAndMiners ( Chain& chain, SimpleMiner* miners, size_t nMiners ) {

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
    
    chain.reset ();
    chain.pushBlock ( genesisBlock );
}

//----------------------------------------------------------------//
TEST ( Chain, test0 ) {

    TheContext::get ().setScoringMode ( TheContext::ScoringMode::INTEGER );
    
    Chain chain;
    SimpleMiner miners [ 2 ];
    initializeTestChainAndMiners ( chain, miners, 2 );
    
    ASSERT_TRUE ( chain.countCycles () == 1 );
    ASSERT_TRUE ( chain.countBlocks ( 0 ) == 1 );
    ASSERT_TRUE ( chain.countBlocks () == 1 );
    
    // cycle 1
    printf ( "\nCYCLE1:\n" );
    miners [ 0 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 1 );
    
    miners [ 1 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 2 );
    
    // cycle 2
    printf ( "\nCYCLE1:\n" );
    miners [ 0 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 1 );
    
    miners [ 1 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 2 );
    
    // cycle 3
    printf ( "\nCYCLE1:\n" );
    miners [ 0 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 4 );
    ASSERT_TRUE ( chain.countBlocks ( 3 ) == 1 );
    
    miners [ 1 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 4 );
    ASSERT_TRUE ( chain.countBlocks ( 3 ) == 2 );
   
    ASSERT_TRUE ( chain.countCycles () == 4 );
    ASSERT_TRUE ( chain.countBlocks ( 0 ) == 1 ); // genesis cycle
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 3 ) == 2 );
}

//----------------------------------------------------------------//
TEST ( Chain, test1 ) {

    TheContext::get ().setScoringMode ( TheContext::ScoringMode::INTEGER );

    Chain chain;
    SimpleMiner miners [ 4 ];
    initializeTestChainAndMiners ( chain, miners, 4 );
    
    // cycle 1
    
    // 1.0 in reverse order
    miners [ 3 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 1 );
    
    miners [ 2 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 1 );
    
    miners [ 1 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 1 );
    
    miners [ 0 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 1 );
    
    // 1.1 in reverse order
    miners [ 3 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 2 );
    
    miners [ 2 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 2 );
    
    miners [ 1 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 2 );
    
    // 1.2 in reverse order
    miners [ 3 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 3 );
    
    miners [ 2 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 3 );
    
    // 1.3 in reverse order
    miners [ 3 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 4 );
    
    // cycle 2
    
    // 2.0 in reverse order
    miners [ 3 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 1 );
    
    miners [ 2 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 1 );
    
    miners [ 1 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 1 );
    
    miners [ 0 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 1 );
    
    // 2.1 in reverse order
    miners [ 3 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 2 );
    
    miners [ 2 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 2 );
    
    miners [ 1 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 2 );
    
    // 2.2 in reverse order
    miners [ 3 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 3 );
    
    miners [ 2 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 3 );
    
    // 2.3 in reverse order
    miners [ 3 ].pushBlock ( chain );
    ASSERT_TRUE ( chain.countCycles () == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 4 );
    
    ASSERT_TRUE ( chain.countCycles () == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 0 ) == 1 ); // genesis cycle
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 4 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 4 );
}

//----------------------------------------------------------------//
TEST ( Chain, test3 ) {

    TheContext::get ().setScoringMode ( TheContext::ScoringMode::INTEGER );

    stringstream strStream;

    {
        Chain chain;
        SimpleMiner miners [ 3 ];
        initializeTestChainAndMiners ( chain, miners, 3 );
        
        miners [ 0 ].pushBlock ( chain );
        miners [ 1 ].pushBlock ( chain );
        miners [ 2 ].pushBlock ( chain );
        
        miners [ 0 ].pushBlock ( chain );
        miners [ 1 ].pushBlock ( chain );
        miners [ 2 ].pushBlock ( chain );
        
        ASSERT_TRUE ( chain.countCycles () == 3 );
        ASSERT_TRUE ( chain.countBlocks ( 0 ) == 1 ); // genesis cycle
        ASSERT_TRUE ( chain.countBlocks ( 1 ) == 3 );
        ASSERT_TRUE ( chain.countBlocks ( 2 ) == 3 );

        ASSERT_TRUE ( chain.print () == "[.][0,1,2 (3)][0,1,2 (3)]" );

        ToJSONSerializer::toJSON ( chain, strStream );
    }
    
    Chain chain;
    FromJSONSerializer::fromJSON ( chain, strStream );
 
    ASSERT_TRUE ( chain.countCycles () == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 0 ) == 1 ); // genesis cycle
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 3 );
    
    ASSERT_TRUE ( chain.print () == "[.][0,1,2 (3)][0,1,2 (3)]" );
}
