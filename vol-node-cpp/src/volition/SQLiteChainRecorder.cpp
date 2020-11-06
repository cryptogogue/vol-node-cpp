// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/FileSys.h>
#include <volition/Miner.h>
#include <volition/SQLiteChainRecorder.h>

namespace Volition {

//================================================================//
// Miner
//================================================================//

//----------------------------------------------------------------//
SQLiteChainRecorder::SQLiteChainRecorder ( const Miner& miner, string path ) {
    
    const Ledger& ledger = miner.getWorkingLedger ();

    shared_ptr < const Block > genesisBlock = ledger.getBlock ( 0 );
    assert ( genesisBlock );
    string hash = genesisBlock->getDigest ();
    
    string filename = Format::write ( "%s/%s.db", path.c_str (), hash.c_str ());
    
    this->mDB.open ( filename );
    
    this->mDB.exec ( "CREATE TABLE IF NOT EXISTS chain ( height INTEGER PRIMARY KEY, hash TEXT NOT NULL, body TEXT NOT NULL )" );
    this->mDB.exec ( "CREATE TABLE IF NOT EXISTS config ( id INTEGER PRIMARY KEY, body TEXT NOT NULL )" );
}

//----------------------------------------------------------------//
SQLiteChainRecorder::~SQLiteChainRecorder () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void SQLiteChainRecorder::AbstractChainRecorder_loadChain ( Miner& miner ) {
    UNUSED ( miner );
    
    const Ledger& ledger = miner.getWorkingLedger ();
    assert ( ledger.countBlocks () == 1 );

    this->mDB.exec (
        "SELECT * FROM chain",
        NULL,
        [ &miner ]( int row, const map < string, int >& columns, sqlite3_stmt* stmt ) {
        
            sqlite_int64 height = sqlite3_column_int64 ( stmt, columns.find ( "height" )->second );
            string hash = ( cc8* )sqlite3_column_text ( stmt, columns.find ( "hash" )->second );
            string body = ( cc8* )sqlite3_column_text ( stmt, columns.find ( "body" )->second );
            
            assert ( height == row );
            if ( row == 0 ) {
                const Ledger& ledger = miner.getWorkingLedger ();
                shared_ptr < const Block > genesisBlock = ledger.getBlock ( 0 );
                assert ( genesisBlock );
                assert ( genesisBlock->getDigest ().toHex () == hash );
            }
            else {
                shared_ptr < Block > block = make_shared < Block >();
                FromJSONSerializer::fromJSONString ( *block, body );
                AbstractChainRecorder::pushBlock ( miner, block );
            }
        }
    );
}

//----------------------------------------------------------------//
void SQLiteChainRecorder::AbstractChainRecorder_loadConfig ( MinerConfig& minerConfig ) {

    this->mDB.exec (
        "SELECT * FROM config WHERE id = 0",
        NULL,
        [ &minerConfig ]( int row, const map < string, int >& columns, sqlite3_stmt* stmt ) {
            UNUSED ( row );
            string body = ( cc8* )sqlite3_column_text ( stmt, columns.find ( "body" )->second );
            FromJSONSerializer::fromJSONString ( minerConfig, body );
        }
    );
}

//----------------------------------------------------------------//
void SQLiteChainRecorder::AbstractChainRecorder_reset () {

    this->mDB.exec ( "DELETE FROM chain" );
}

//----------------------------------------------------------------//
void SQLiteChainRecorder::AbstractChainRecorder_saveChain ( const Miner& miner ) {
    UNUSED ( miner );

    BlockTreeNode::ConstPtr cursor = miner.getChainTag ();

    for ( ; cursor; cursor = cursor->getParent ()) {
    
        shared_ptr < const Block > block = cursor->getBlock ();
        assert ( block );
    
        size_t height = block->getHeight ();
    
        string storedHash;
        this->mDB.exec (
            Format::write ( "SELECT * FROM chain WHERE height = ?1", height ),
            [ height ]( sqlite3_stmt* stmt ) {
                sqlite3_bind_int64 ( stmt, 1, ( sqlite_int64 )height );
            },
            [ &storedHash, height ]( int row, const map < string, int >& columns, sqlite3_stmt* stmt ) {
                UNUSED ( row );
                storedHash = ( cc8* )sqlite3_column_text ( stmt, columns.find ( "hash" )->second );
            }
        );
        
        string hash = block->getDigest ().toHex ();
        
        if ( storedHash == hash ) break;
        string body = ToJSONSerializer::toJSONString ( *block );
        
        this->mDB.exec (
            Format::write ( "REPLACE INTO chain ( height, hash, body ) VALUES ( ?1, ?2, ?3 )" ),
            [ height, hash, &body ]( sqlite3_stmt* stmt ) {
                sqlite3_bind_int64 ( stmt, 1, ( sqlite_int64 )height );
                sqlite3_bind_text ( stmt, 2, hash.c_str (), ( int )hash.size (), SQLITE_TRANSIENT );
                sqlite3_bind_text ( stmt, 3, body.c_str (), ( int )body.size (), SQLITE_TRANSIENT );
            }
        );
    }
}

//----------------------------------------------------------------//
void SQLiteChainRecorder::AbstractChainRecorder_saveConfig ( const MinerConfig& minerConfig ) {

    string body = ToJSONSerializer::toJSONString ( minerConfig );

    this->mDB.exec (
        Format::write ( "REPLACE INTO config ( id, body ) VALUES ( 0, ?1 )" ),
        [ &body ]( sqlite3_stmt* stmt ) {
            sqlite3_bind_text ( stmt, 1, body.c_str (), ( int )body.size (), SQLITE_TRANSIENT );
        }
    );
}

} // namespace Volition
