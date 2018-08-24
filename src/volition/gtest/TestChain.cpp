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

//----------------------------------------------------------------//
void pushBlock ( Chain& chain, string minerID, const CryptoKey& key ) {

    shared_ptr < Block > block = make_shared < Block >();
    block->setMinerID ( minerID );
    
    ChainPlacement placement = chain.findPlacement ( minerID, true );
    ASSERT_TRUE ( placement.canPush ());
    
    chain.pushAndSign ( placement, block, key );
}

//----------------------------------------------------------------//
void pushMinerGenesisTransaction ( Block& block, string minerID, const CryptoKey& key  ) {

    unique_ptr < Transaction::GenesisMiner > genesisMinerTransaction = make_unique < Transaction::GenesisMiner >();
    
    genesisMinerTransaction->mAccountName = minerID;
    genesisMinerTransaction->mKey = key;
    genesisMinerTransaction->mKeyName = "master";
    genesisMinerTransaction->mAmount = 0;
    genesisMinerTransaction->mURL = "";

    block.pushTransaction ( move ( genesisMinerTransaction ));
}

//----------------------------------------------------------------//
TEST ( Chain, unitTests ) {

    string miner0 = "0";
    CryptoKey key0;
    key0.elliptic ();
    
    string miner1 = "1";
    CryptoKey key1;
    key1.elliptic ();

    shared_ptr < Block > genesisBlock = make_shared < Block >();

    pushMinerGenesisTransaction ( *genesisBlock, miner0, key0 );
    pushMinerGenesisTransaction ( *genesisBlock, miner1, key1 );

    CryptoKey genesisKey;
    genesisKey.elliptic ();
    TheContext::get ().setGenesisBlockKey ( genesisKey );
    
    genesisBlock->sign ( genesisKey );
    TheContext::get ().setGenesisBlockDigest ( genesisBlock->getSignature ().getDigest ());
    
    Chain chain ( genesisBlock );
    
    // cycle 0
    pushBlock ( chain, miner0, key0 );
    pushBlock ( chain, miner1, key1 );
    
    // cycle 1
    pushBlock ( chain, miner0, key0 );
    pushBlock ( chain, miner1, key1 );
    
    ASSERT_TRUE ( chain.countCycles () == 3 );
    ASSERT_TRUE ( chain.countBlocks ( 0 ) == 1 ); // genesis cycle
    ASSERT_TRUE ( chain.countBlocks ( 1 ) == 2 );
    ASSERT_TRUE ( chain.countBlocks ( 2 ) == 2 );
}
