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
void SQLiteChainRecorder::exec ( sqlite3_stmt* stmt, SQLiteChainRecorder::SQLRowCallbackFunc onRow ) {

    int rows = 0;
    map < string, int > columns;
    
    while ( true ) {
        
        int result = sqlite3_step ( stmt );
        if ( result != SQLITE_ROW ) break;
        
        if ( rows == 0 ){
            int nCols = sqlite3_column_count ( stmt );
            for ( int i = 0; i < nCols; i++ ) {
                cc8* name = ( char* )sqlite3_column_name ( stmt, i );
                columns [ name ] = i;
            }
        }
        if ( onRow ) {
            onRow ( rows, columns, stmt );
        }
        rows++;
    }
    sqlite3_reset ( stmt );
}

//----------------------------------------------------------------//
void SQLiteChainRecorder::exec ( string sql, SQLPrepareCallbackFunc onPrepare, SQLiteChainRecorder::SQLRowCallbackFunc onRow ) {
    
    sqlite3_stmt* stmt = this->prepare ( sql, onPrepare );
    if ( !stmt ) return;

    this->exec ( stmt, onRow );
    sqlite3_finalize ( stmt );
}

//----------------------------------------------------------------//
sqlite3_stmt* SQLiteChainRecorder::prepare ( string sql, SQLPrepareCallbackFunc onPrepare ) {

    sqlite3_stmt* stmt = NULL;
    int result = sqlite3_prepare_v2 (
        this->mDB,
        sql.c_str (),
        ( int )sql.size (),
        &stmt,
        NULL
    );
    
    if ( result != SQLITE_OK ) {
        sqlite3_finalize ( stmt );
        stmt = NULL;
    }
    
    if ( stmt && onPrepare ) {
        onPrepare ( stmt );
    }
    
    return stmt;
}

//----------------------------------------------------------------//
SQLiteChainRecorder::SQLiteChainRecorder ( const Miner& miner, string path ) :
    mDB ( NULL ) {
    
    const Ledger& ledger = miner.getLedger ();

    shared_ptr < Block > genesisBlock = ledger.getBlock ( 0 );
    assert ( genesisBlock );
    string hash = genesisBlock->getDigest ();
    
    string filename = Format::write ( "%s/%s.db", path.c_str (), hash.c_str ());
    
    int result = sqlite3_open ( filename.c_str (), &this->mDB );
    if ( result ) {
        sqlite3_close ( this->mDB );
        this->mDB = NULL;
        assert ( false );
        return;
    }
    
    this->exec ( "CREATE TABLE IF NOT EXISTS chain ( height INTEGER PRIMARY KEY, hash TEXT NOT NULL, body TEXT NOT NULL )" );
    this->exec ( "CREATE TABLE IF NOT EXISTS config ( id INTEGER PRIMARY KEY, body TEXT NOT NULL )" );
}

//----------------------------------------------------------------//
SQLiteChainRecorder::~SQLiteChainRecorder () {

    sqlite3_close ( this->mDB );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void SQLiteChainRecorder::AbstractChainRecorder_loadChain ( Miner& miner ) {
    UNUSED ( miner );
    
    const Ledger& ledger = miner.getLedger ();
    assert ( ledger.countBlocks () == 1 );

    this->exec (
        "SELECT * FROM chain",
        NULL,
        [ &miner ]( int row, const map < string, int >& columns, sqlite3_stmt* stmt ) {
        
            sqlite_int64 height = sqlite3_column_int64 ( stmt, columns.find ( "height" )->second );
            string hash = ( cc8* )sqlite3_column_text ( stmt, columns.find ( "hash" )->second );
            string body = ( cc8* )sqlite3_column_text ( stmt, columns.find ( "body" )->second );
            
            assert ( height == row );
            if ( row == 0 ) {
                const Ledger& ledger = miner.getLedger ();
                shared_ptr < Block > genesisBlock = ledger.getBlock ( 0 );
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

    this->exec (
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

    this->exec ( "DELETE FROM chain" );
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
        this->exec (
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
        
        this->exec (
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

    this->exec (
    Format::write ( "REPLACE INTO config ( id, body ) VALUES ( 0, ?1 )" ),
    [ &body ]( sqlite3_stmt* stmt ) {
        sqlite3_bind_text ( stmt, 1, body.c_str (), ( int )body.size (), SQLITE_TRANSIENT );
    }
);
}

} // namespace Volition
