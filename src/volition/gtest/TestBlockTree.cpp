// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/BlockHeader.h>
#include <volition/CryptoKeyPair.h>
#include <volition/Digest.h>
#include <volition/FileSys.h>
#include <volition/InMemoryBlockTree.h>
#include <volition/Miner.h>
#include <volition/Release.h>
#include <volition/SQLiteBlockTree.h>

using namespace Volition;

static cc8* SQLITE_FILE     = "sqlite-blocktree-test.db";

//----------------------------------------------------------------//
shared_ptr < Block >    makeBlock           ( string minerID, const Digest& visage, time_t now, const BlockHeader* prevBlock, const CryptoKeyPair& key );
void                    standardTest        ( AbstractBlockTree& tree );

//----------------------------------------------------------------//
shared_ptr < Block > makeBlock ( string minerID, const Digest& visage, time_t now, const BlockHeader* prevBlock, const CryptoKeyPair& key ) {

    shared_ptr < Block > block = make_shared < Block >();
    block->initialize ( minerID, VOL_NODE_RELEASE, visage, now, prevBlock, key );
    block->sign ( key );
    return block;
}

//----------------------------------------------------------------//
void standardTest ( AbstractBlockTree& tree ) {

    string minerID = "9090";
    CryptoKeyPair keyPair;
    keyPair.elliptic ();
    Signature visage = Miner::calculateVisage ( keyPair );

    shared_ptr < Block > block_A        = makeBlock ( minerID, visage, 0, NULL, keyPair );
    
    shared_ptr < Block > block_AB       = makeBlock ( minerID, visage, 0, block_A.get (), keyPair );
    shared_ptr < Block > block_ABC      = makeBlock ( minerID, visage, 0, block_AB.get (), keyPair );
    
    shared_ptr < Block > block_AD       = makeBlock ( minerID, visage, 0, block_A.get (), keyPair );
    shared_ptr < Block > block_ADE      = makeBlock ( minerID, visage, 0, block_AD.get (), keyPair );
    
    BlockTreeTag master ( "master" );
    BlockTreeTag branch ( "branch" );
    BlockTreeCursor cursor;
    
    cursor = tree.affirmHeader ( master, block_A );
    ASSERT_EQ ( cursor.hasHeader (), true );
    
    cursor = tree.affirmHeader ( master, block_AB );
    ASSERT_EQ ( cursor.hasHeader (), true );
    
    cursor = tree.affirmHeader ( master, block_ABC );
    ASSERT_EQ ( cursor.hasHeader (), true );
    
    cursor = tree.affirmHeader ( branch, block_AD );
    ASSERT_EQ ( cursor.hasHeader (), true );
    
    cursor = tree.affirmHeader ( branch, block_ADE );
    ASSERT_EQ ( cursor.hasHeader (), true );
    
    tree.tag ( master, branch );
}

////----------------------------------------------------------------//
//TEST ( BlockTree, inMemory ) {
//
//    InMemoryBlockTree tree;
//    standardTest ( tree );
//}

//----------------------------------------------------------------//
TEST ( BlockTree, sqlite ) {

    if ( FileSys::exists ( SQLITE_FILE )) {
        remove ( SQLITE_FILE );
    }
    ASSERT_EQ ( FileSys::exists ( SQLITE_FILE ), false );

    SQLiteBlockTree tree ( SQLITE_FILE, SQLiteConfig ());
    standardTest ( tree );
    
//    ASSERT_EQ ( remove ( SQLITE_FILE ), 0 );
//    ASSERT_EQ ( FileSys::exists ( SQLITE_FILE ), false );
}
