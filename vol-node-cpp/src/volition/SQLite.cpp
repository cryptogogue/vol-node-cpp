// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/FileSys.h>
#include <volition/Miner.h>
#include <volition/SQLite.h>

namespace Volition {

//================================================================//
// SQLite
//================================================================//

//----------------------------------------------------------------//
void SQLite::close () {

    if ( this->mDB ) {
        sqlite3_close ( this->mDB );
    }
}

//----------------------------------------------------------------//
void SQLite::exec ( sqlite3_stmt* stmt, SQLRowCallbackFunc onRow ) {

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
void SQLite::exec ( string sql, SQLPrepareCallbackFunc onPrepare, SQLRowCallbackFunc onRow ) {
    
    sqlite3_stmt* stmt = this->prepare ( sql, onPrepare );
    if ( !stmt ) return;

    this->exec ( stmt, onRow );
    sqlite3_finalize ( stmt );
}

//----------------------------------------------------------------//
sqlite3_stmt* SQLite::prepare ( string sql, SQLPrepareCallbackFunc onPrepare ) {

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
void SQLite::open ( string filename ) {

    int result = sqlite3_open ( filename.c_str (), &this->mDB );
    if ( result ) {
        sqlite3_close ( this->mDB );
        this->mDB = NULL;
    }
}

//----------------------------------------------------------------//
SQLite::SQLite () :
    mDB ( NULL ) {
}

//----------------------------------------------------------------//
SQLite::SQLite ( string filename ) :
    mDB ( NULL ) {
    
    this->open ( filename );
}


//----------------------------------------------------------------//
SQLite::~SQLite () {

    this->close ();
}

} // namespace Volition
