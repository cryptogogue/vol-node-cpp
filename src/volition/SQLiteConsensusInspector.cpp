// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/SQLiteConsensusInspector.h>

namespace Volition {

//================================================================//
// SQLiteConsensusInspector
//================================================================//

//----------------------------------------------------------------//
SQLiteConsensusInspector::SQLiteConsensusInspector ( string filename ) {

    SQLiteResult result = this->mDB.open ( filename );
    assert ( result );
}

//----------------------------------------------------------------//
SQLiteConsensusInspector::~SQLiteConsensusInspector () {
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
shared_ptr < const Block > SQLiteConsensusInspector::AbstractConsensusInspector_getBlock ( string hash ) const {

    shared_ptr < Block > block;

    this->mDB.exec (
        
        "SELECT block FROM nodes WHERE hash IS ?1",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, hash );
        },
        
        //--------------------------------//
        [ & ]( int, const SQLiteStatement& stmt ) {
        
            string blockJSON = stmt.getValue < string >( "block" );

            assert ( blockJSON.size ());
            
            block = make_shared < Block >();
            FromJSONSerializer::fromJSONString ( *block, blockJSON );
        }
    );

    return block;
}

} // namespace Volition
