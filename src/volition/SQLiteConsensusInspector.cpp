// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/SQLiteConsensusInspector.h>

namespace Volition {

//================================================================//
// SQLiteConsensusInspector
//================================================================//

//----------------------------------------------------------------//
SQLiteConsensusInspector::SQLiteConsensusInspector ( string filename ) :
    mFilename ( filename ){
}

//----------------------------------------------------------------//
SQLiteConsensusInspector::~SQLiteConsensusInspector () {
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
shared_ptr < const Block > SQLiteConsensusInspector::AbstractConsensusInspector_getBlock ( string hash ) const {

    SQLite db;
    SQLiteResult result = db.open ( this->mFilename, SQLITE_OPEN_READONLY );
    if ( !result ) return NULL;

    shared_ptr < Block > block;

    result = db.exec (
        
        "SELECT block FROM nodes WHERE hash IS ?1 AND searchStatus IS '#'",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, hash );
        },
        
        //--------------------------------//
        [ & ]( int, const SQLiteStatement& stmt ) {
        
            string blockJSON = stmt.getValue < string >( "block" );

            if ( blockJSON.size ()) {
                block = make_shared < Block >();
                FromJSONSerializer::fromJSONString ( *block, blockJSON );
            }
        }
    );
    result.reportWithAssert ();

    return block;
}

} // namespace Volition
