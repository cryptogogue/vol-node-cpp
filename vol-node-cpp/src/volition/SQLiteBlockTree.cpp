// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/SQLiteBlockTree.h>

#define SQL_STR(...) #__VA_ARGS__

namespace Volition {

//================================================================//
// SQLiteBlockTree
//================================================================//

//----------------------------------------------------------------//
kBlockTreeEntryMeta SQLiteBlockTree::stringToMeta ( string str ) {

    char c = str.size () ? str [ 0 ] : 0;

    switch ( c ) {
        case '*':       return kBlockTreeEntryMeta::META_PROVISIONAL;
        case '#':       return kBlockTreeEntryMeta::META_REFUSED;
        case '.':
        default:        break;
    }
    return kBlockTreeEntryMeta::META_NONE;
}

//----------------------------------------------------------------//
kBlockTreeEntryStatus SQLiteBlockTree::stringToStatus ( string str ) {

    char c = str.size () ? str [ 0 ] : 0;

    switch ( c ) {
        case 'N':       return kBlockTreeEntryStatus::STATUS_NEW;
        case 'C':       return kBlockTreeEntryStatus::STATUS_COMPLETE;
        case '?':       return kBlockTreeEntryStatus::STATUS_MISSING;
        case 'X':
        default:        break;
    }
    return kBlockTreeEntryStatus::STATUS_INVALID;
}

//----------------------------------------------------------------//
SQLiteBlockTree::SQLiteBlockTree ( string filename ) {

    SQLiteResult result = this->mDB.open ( filename );
    assert ( result );
    
    result = this->mDB.exec ( SQL_STR (
        CREATE TABLE IF NOT EXISTS nodes (
            hash        TEXT                                                NOT NULL,
            header      TEXT                                                NOT NULL,
            block       TEXT,
            status      TEXT CHECK ( status IN ( 'N', 'C', '?', 'X' ))      NOT NULL DEFAULT 'N',
            meta        TEXT CHECK ( status IN ( '.','*','#' ))             NOT NULL DEFAULT '.',
            refcount    INT                                                 NOT NULL DEFAULT 0
        )
    ));
    assert ( result );
    
    result = this->mDB.exec ( SQL_STR (
        CREATE UNIQUE INDEX IF NOT EXISTS hash ON nodes ( hash )
    ));
    assert ( result );
    
    result = this->mDB.exec ( SQL_STR (
        CREATE TABLE IF NOT EXISTS closure (
            parent  INTEGER                                             NOT NULL,
            child   INTEGER                                             NOT NULL,
            depth   INTEGER                                             NOT NULL,
            FOREIGN KEY ( parent )  REFERENCES nodes ( rowid ),
            FOREIGN KEY ( child )   REFERENCES nodes ( rowid )
        )
    ));
    assert ( result );
}

//----------------------------------------------------------------//
SQLiteBlockTree::~SQLiteBlockTree () {
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
BlockTreeCursor SQLiteBlockTree::AbstractBlockTree_affirm ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional ) {
    UNUSED ( tag );
    UNUSED ( header );
    UNUSED ( block );
    UNUSED ( isProvisional );

    return BlockTreeCursor ();
}

//----------------------------------------------------------------//
kBlockTreeAppendResult SQLiteBlockTree::AbstractBlockTree_checkAppend ( const BlockHeader& header ) const {
     UNUSED ( header );

    return APPEND_OK;
}

//----------------------------------------------------------------//
int SQLiteBlockTree::AbstractBlockTree_compare ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1, kRewriteMode rewriteMode ) const {
    UNUSED ( cursor0 );
    UNUSED ( cursor1 );
    UNUSED ( rewriteMode );

    return 0;
}

//----------------------------------------------------------------//
BlockTreeCursor SQLiteBlockTree::AbstractBlockTree_findCursorForHash ( string hash ) const {
    UNUSED ( hash );

    BlockTreeCursor cursor;

    this->mDB.exec (
        "SELECT * FROM nodes WHERE hash = ?1",
        
        [ hash ]( sqlite3_stmt* stmt ) {
            sqlite3_bind_text ( stmt, 1, hash.c_str (), ( int )hash.size (), SQLITE_TRANSIENT );
        },
        
        [ this, &cursor ]( int row, const map < string, int >& columns, sqlite3_stmt* stmt ) {
            UNUSED ( row );
        
            string hash             = ( cc8* )sqlite3_column_text ( stmt, columns.find ( "hash" )->second );
            string headerJSON       = ( cc8* )sqlite3_column_text ( stmt, columns.find ( "header" )->second );
            string blockJSON        = ( cc8* )sqlite3_column_text ( stmt, columns.find ( "block" )->second );
            string statusStr        = ( cc8* )sqlite3_column_text ( stmt, columns.find ( "status" )->second );
            string metaStr          = ( cc8* )sqlite3_column_text ( stmt, columns.find ( "meta" )->second );

            // header *must* exist.
            assert ( headerJSON.size ());
            
            shared_ptr < BlockHeader >header;
            shared_ptr < Block >block;
            
            // if there's a block, use that as the header, too.
            if ( blockJSON.size ()) {
                block = make_shared < Block >();
                FromJSONSerializer::fromJSONString ( *block, headerJSON );
                header = block;
            }
            else {
                // no block, only header.
                header = make_shared < BlockHeader >();
                FromJSONSerializer::fromJSONString ( *header, headerJSON );
            }

            cursor = this->makeCursor (
                header,
                block,
                stringToStatus ( statusStr ),
                stringToMeta ( metaStr )
            );
        }
    );

    return BlockTreeCursor ();
}

//----------------------------------------------------------------//
BlockTreeCursor SQLiteBlockTree::AbstractBlockTree_findCursorForTag ( const BlockTreeTag& tag ) const {
    UNUSED ( tag );

    return BlockTreeCursor ();
}

//----------------------------------------------------------------//
BlockTreeCursor SQLiteBlockTree::AbstractBlockTree_findRoot ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const {
    UNUSED ( cursor0 );
    UNUSED ( cursor1 );

    return BlockTreeCursor ();
}

//----------------------------------------------------------------//
void SQLiteBlockTree::AbstractBlockTree_mark ( const BlockTreeCursor& cursor, kBlockTreeEntryStatus status ) {
    UNUSED ( cursor );
    UNUSED ( status );
}

//----------------------------------------------------------------//
BlockTreeCursor SQLiteBlockTree::AbstractBlockTree_tag ( BlockTreeTag& tag, const BlockTreeCursor& cursor ) {
    UNUSED ( tag );
    UNUSED ( cursor );

    return BlockTreeCursor ();
}

//----------------------------------------------------------------//
BlockTreeCursor SQLiteBlockTree::AbstractBlockTree_tag ( BlockTreeTag& tag, const BlockTreeTag& otherTag ) {
    UNUSED ( tag );
    UNUSED ( otherTag );

    return BlockTreeCursor ();
}

//----------------------------------------------------------------//
BlockTreeCursor SQLiteBlockTree::AbstractBlockTree_update ( shared_ptr < const Block > block ) {
    UNUSED ( block );

    return BlockTreeCursor ();
}

} // namespace Volition
