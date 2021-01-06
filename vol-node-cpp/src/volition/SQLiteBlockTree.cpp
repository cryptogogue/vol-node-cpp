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
int SQLiteBlockTree::getNodeIDFromHash ( string hash ) {

    int nodeID = 0;
    
    SQLiteResult result = this->mDB.exec (
        
        "SELECT rowid FROM nodes WHERE hash IS ?1",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, hash );
        },
        
        //--------------------------------//
        [ & ]( int, const SQLiteStatement& stmt ) {
            nodeID = stmt.getValue < int >( 0 );
        }
    );
    assert ( result );
    
    return nodeID;
}

//----------------------------------------------------------------//
int SQLiteBlockTree::getNodeIDFromTagName ( string tagName ) {

    int nodeID = 0;
    
    SQLiteResult result = this->mDB.exec (
        
        "SELECT nodeID FROM tags WHERE name IS ?1",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, tagName );
        },
        
        //--------------------------------//
        [ & ]( int, const SQLiteStatement& stmt ) {
            nodeID = stmt.getValue < int >( 0 );
        }
    );
    assert ( result );
    
    return nodeID;
}

//----------------------------------------------------------------//
kBlockTreeEntryStatus SQLiteBlockTree::getNodeStatus ( int nodeID, kBlockTreeEntryStatus status ) {
            
    SQLiteResult result = this->mDB.exec (
        
        "SELECT status FROM nodes WHERE rowid IS ?1",
        
        //--------------------------------//
        [ & ]( SQLiteStatement stmt ) {
            stmt.bind ( 1, nodeID );
        },
        
        //--------------------------------//
        [ & ]( int, SQLiteStatement stmt ) {
            status = stringToStatus ( stmt.getValue < string >( 0 ));
        }
    );
    assert ( result );
    
    return status;
}

//----------------------------------------------------------------//
void SQLiteBlockTree::markRecurse ( int blockID, kBlockTreeEntryStatus status ) {

    SQLiteResult result;

    kBlockTreeEntryStatus prevStatus;
    kBlockTreeEntryStatus prevParentStatus;
    
    size_t height   = 0;
    bool hasBlock   = false;
    bool exists     = false;
    
    // first, get some information about the node as it exists now.
    result = this->mDB.exec (
    
        "SELECT height, status, parentStatus, hasBlock FROM nodes WHERE rowid IS ?1",
    
        //--------------------------------//
        [ & ]( SQLiteStatement stmt ) {
            stmt.bind ( 1, blockID );
        },
        
        //--------------------------------//
        [ & ]( int, SQLiteStatement stmt ) {
            height              = ( size_t )stmt.getValue < int >( 0 );
            prevStatus          = SQLiteBlockTree::stringToStatus ( stmt.getValue < string >( 1 ));
            prevParentStatus    = SQLiteBlockTree::stringToStatus ( stmt.getValue < string >( 2 ));
            hasBlock            = stmt.getValue < int >( 0 ) != 0;
            exists              = true;
        }
    );
    assert ( result );

    // if we couldn't find it, or we did find it and the status matches what we want to set, we can bail.
    if ( !exists || ( status == prevParentStatus )) return;

    // if we're setting the status to complete, then there has to be a block and the parent status also has to be complete.
    if ( status == STATUS_COMPLETE ) {
        if ( !hasBlock ) return;
        if (( height > 0 ) && ( prevParentStatus != STATUS_COMPLETE )) return;
    }

    // sanity check.
    assert ( AbstractBlockTree::checkStatusTransition ( prevStatus, status ));
    
    // go ahead and update the status.
    result = this->mDB.exec (
    
        "UPDATE nodes SET status = ?1 WHERE rowid IS ?2",
    
        //--------------------------------//
        [ & ]( SQLiteStatement stmt ) {
            stmt.bind ( 1,  SQLiteBlockTree::stringFromStatus ( status ) );
            stmt.bind ( 2,  blockID );
        }
    );
    assert ( result );
    
    // set parentStatus for all child nodes.
    result = this->mDB.exec (
    
        "UPDATE nodes SET parentStatus = ?1 WHERE parentID IS ?2",
        
        //--------------------------------//
        [ & ]( SQLiteStatement stmt ) {
            stmt.bind ( 1, SQLiteBlockTree::stringFromStatus ( status ));
            stmt.bind ( 2, blockID );
        }
    );
    assert ( result );
    
    // recursively mark the child nodes.
    result = this->mDB.exec (
    
        "SELECT rowid FROM nodes WHERE parentID IS ?1",
    
        //--------------------------------//
        [ & ]( SQLiteStatement stmt ) {
            stmt.bind ( 1, blockID );
        },
        
        //--------------------------------//
        [ & ]( int, SQLiteStatement stmt ) {
            int childID = stmt.getValue < int >( 0 );
            this->markRecurse ( childID, status );
        }
    );
    assert ( result );
}

//----------------------------------------------------------------//
BlockTreeCursor SQLiteBlockTree::readCursor ( const SQLiteStatement& stmt ) const {

    string hash             = stmt.getValue < string >( "hash" );
    string headerJSON       = stmt.getValue < string >( "header" );
    string blockJSON        = stmt.getValue < string >( "block" );
    string statusStr        = stmt.getValue < string >( "status" );
    string metaStr          = stmt.getValue < string >( "meta" );

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

    return this->makeCursor (
        header,
        block,
        stringToStatus ( statusStr ),
        stringToMeta ( metaStr )
    );
}

//----------------------------------------------------------------//
void SQLiteBlockTree::releaseNode ( int nodeID ) {

    if ( nodeID == 0 ) return;

    SQLiteResult result;

    result = this->mDB.exec (
            
        "UPDATE nodes SET refCount = refCount - 1 WHERE rowid IS ?1",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, nodeID );
        }
    );
    assert ( result );
    
    // SELECT rowid FROM nodes WHERE rowid NOT IN ( SELECT nodeID FROM tags WHERE nodeID IS nodes.rowID UNION SELECT rowID FROM nodes WHERE parentID IS nodes.rowID )

    
//    // and delete anything with a 0 refCount
//    result = this->mDB.exec (
//        "DELETE FROM nodes WHERE refCount = 0"
//    );
    assert ( result );
}

//----------------------------------------------------------------//
void SQLiteBlockTree::retainNode ( int nodeID ) {

    if ( nodeID == 0 ) return;

    // *increment* the incoming target's refCount
    SQLiteResult result = this->mDB.exec (
    
        "UPDATE nodes SET refCount = refCount + 1 WHERE rowid IS ?1",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, nodeID );
        }
    );
    assert ( result );
}

//----------------------------------------------------------------//
void SQLiteBlockTree::setTag ( string tagName, int nodeID ) {

    if ( !nodeID ) return;

    int prevNodeID = this->getNodeIDFromTagName ( tagName );
    
    if ( prevNodeID == nodeID ) return;
        
    this->retainNode ( nodeID );
    this->releaseNode ( prevNodeID );

    SQLiteResult result = this->mDB.exec (
        
        "INSERT INTO tags ( name, nodeID ) VALUES ( ?1, ?2 ) ON CONFLICT ( name ) DO UPDATE SET nodeID = ?2",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, tagName );
            stmt.bind ( 2, nodeID );
        }
    );
    assert ( result );
}

//----------------------------------------------------------------//
string SQLiteBlockTree::stringFromMeta ( kBlockTreeEntryMeta meta ) {

    switch ( meta ) {
        case kBlockTreeEntryMeta::META_PROVISIONAL:     return "*";
        case kBlockTreeEntryMeta::META_REFUSED:         return "#";
        case kBlockTreeEntryMeta::META_NONE:            return ".";
    }
    return "";
}

//----------------------------------------------------------------//
string SQLiteBlockTree::stringFromStatus ( kBlockTreeEntryStatus status ) {

    switch ( status ) {
        case kBlockTreeEntryStatus::STATUS_NEW:         return "N";
        case kBlockTreeEntryStatus::STATUS_COMPLETE:    return "C";
        case kBlockTreeEntryStatus::STATUS_MISSING:     return "M";
        case kBlockTreeEntryStatus::STATUS_INVALID:     return "X";
    }
    return "";
}

//----------------------------------------------------------------//
kBlockTreeEntryMeta SQLiteBlockTree::stringToMeta ( string str ) {

    char c = str.size () ? str [ 0 ] : 0;

    switch ( c ) {
        case '*':       return kBlockTreeEntryMeta::META_PROVISIONAL;
        case '#':       return kBlockTreeEntryMeta::META_REFUSED;
        case '.':       return kBlockTreeEntryMeta::META_NONE;
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
        case 'X':       return kBlockTreeEntryStatus::STATUS_INVALID;
    }
    return kBlockTreeEntryStatus::STATUS_INVALID;
}

//----------------------------------------------------------------//
SQLiteBlockTree::SQLiteBlockTree ( string filename ) {

    SQLiteResult result = this->mDB.open ( filename );
    assert ( result );
    
    result = this->mDB.exec ( SQL_STR (
        CREATE TABLE IF NOT EXISTS nodes (
            parentID        INT                                                     NOT NULL DEFAULT 0,
            hash            TEXT                                                    NOT NULL,
            height          INT                                                     NOT NULL DEFAULT 0,
            header          TEXT                                                    NOT NULL,
            block           TEXT,
            status          TEXT CHECK ( status IN ( 'N', 'C', '?', 'X' ))          NOT NULL DEFAULT 'N',
            parentStatus    TEXT CHECK ( parentStatus IN ( 'N', 'C', '?', 'X' ))    NOT NULL DEFAULT 'N',
            meta            TEXT CHECK ( meta IN ( '.', '*', '#' ))                 NOT NULL DEFAULT '.',
            hasBlock        INT                                                     NOT NULL DEFAULT 0,
            refcount        INT                                                     NOT NULL DEFAULT 0
        )
    ));
    assert ( result );
    
    result = this->mDB.exec ( SQL_STR (
        CREATE UNIQUE INDEX IF NOT EXISTS hash ON nodes ( hash )
    ));
    assert ( result );
    
    result = this->mDB.exec ( SQL_STR (
        CREATE INDEX IF NOT EXISTS parentID ON nodes ( parentID )
    ));
    assert ( result );
    
    result = this->mDB.exec ( SQL_STR (
        CREATE TABLE IF NOT EXISTS tags (
            nodeID  INTEGER     NOT NULL,
            name    TEXT        NOT NULL,
            FOREIGN KEY ( nodeID )  REFERENCES nodes ( rowid )
        )
    ));
    assert ( result );
    
    result = this->mDB.exec ( SQL_STR (
        CREATE UNIQUE INDEX IF NOT EXISTS name ON tags ( name )
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
        
    string tagName = tag.getName ();
    string hash = header->getDigest ().toHex ();
    BlockTreeCursor cursor = this->findCursorForHash ( hash );

    if ( cursor.hasHeader ()) {
        if ( block && !cursor.getBlock ()) {
            this->update ( block );
        }
    }
    else {
        
        kBlockTreeEntryStatus status = block ? kBlockTreeEntryStatus::STATUS_COMPLETE : kBlockTreeEntryStatus::STATUS_NEW;
        kBlockTreeEntryStatus parentStatus = kBlockTreeEntryStatus::STATUS_COMPLETE;

        int parentID = header->isGenesis () ? 0 : this->getNodeIDFromHash ( header->getPrevDigest ().toHex ());

        if ( parentID ) {
        
            retainNode ( parentID );
            parentStatus = this->getNodeStatus ( parentID, kBlockTreeEntryStatus::STATUS_INVALID );
            
            if (( parentStatus == kBlockTreeEntryStatus::STATUS_MISSING ) || ( parentStatus == kBlockTreeEntryStatus::STATUS_INVALID )) {
                status = parentStatus;
            }
        }

        // insert node
        SQLiteResult result = this->mDB.exec (
            
            "INSERT INTO nodes ( parentID, hash, height, header, block, status, parentStatus, meta, refCount ) VALUES ( ?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9 )",
            
            //--------------------------------//
            [ & ]( SQLiteStatement stmt ) {
                
                stmt.bind ( 1,      parentID );
                stmt.bind ( 2,      hash );
                stmt.bind ( 3,      ( int )header->getHeight ());
                stmt.bind ( 4,      ToJSONSerializer::toJSONString ( *header ));
                stmt.bind ( 5,      block ? ToJSONSerializer::toJSONString ( *block ) : "" );
                stmt.bind ( 6,      SQLiteBlockTree::stringFromStatus ( status ));
                stmt.bind ( 7,      SQLiteBlockTree::stringFromStatus ( parentStatus ));
                stmt.bind ( 8,      SQLiteBlockTree::stringFromMeta ( isProvisional ? kBlockTreeEntryMeta::META_PROVISIONAL : kBlockTreeEntryMeta::META_NONE ));
                stmt.bind ( 9,      0 );
            }
        );
        assert ( result );
        
        // make the cursor
        cursor = this->makeCursor ( header, block, status, kBlockTreeEntryMeta::META_NONE );
    }

    // tag and return
    return this->tag ( tag, cursor );
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
        
        "SELECT * FROM nodes WHERE hash IS ?1",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, hash );
        },
        
        //--------------------------------//
        [ & ]( int, const SQLiteStatement& stmt ) {
            cursor = this->readCursor ( stmt );
        }
    );

    return cursor;
}

//----------------------------------------------------------------//
BlockTreeCursor SQLiteBlockTree::AbstractBlockTree_findCursorForTag ( const BlockTreeTag& tag ) const {

    BlockTreeCursor cursor;

    string name = tag.getName ();

    this->mDB.exec (
        
        "SELECT * FROM nodes INNER JOIN tags ON tags.nodeID = nodes.rowid WHERE tags.name IS ?1",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, name );
        },
        
        //--------------------------------//
        [ & ]( int, const SQLiteStatement& stmt ) {
            cursor = this->readCursor ( stmt );
        }
    );

    return cursor;
}

//----------------------------------------------------------------//
BlockTreeCursor SQLiteBlockTree::AbstractBlockTree_findRoot ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const {
    UNUSED ( cursor0 );
    UNUSED ( cursor1 );

    return BlockTreeCursor ();
}

//----------------------------------------------------------------//
void SQLiteBlockTree::AbstractBlockTree_mark ( const BlockTreeCursor& cursor, kBlockTreeEntryStatus status ) {

    string hash     = cursor.getDigest ().toHex ();
    int blockID     = 0;
    bool exists     = false;
    
    this->mDB.exec (
        
        "SELECT rowid FROM nodes WHERE hash ID ?1",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, hash );
        },
        
        //--------------------------------//
        [ & ]( int, const SQLiteStatement& stmt ) {
            blockID     = stmt.getValue< int >( 0 );
            exists      = true;
        }
    );
    
    if ( !exists ) return;
    
    this->markRecurse ( blockID, status );
}

//----------------------------------------------------------------//
BlockTreeCursor SQLiteBlockTree::AbstractBlockTree_tag ( BlockTreeTag& tag, const BlockTreeCursor& cursor ) {

    int nodeID = this->getNodeIDFromHash ( cursor.getHash ());
    assert ( nodeID != 0 );

    this->setTag ( tag.getName (), nodeID );
    return cursor;
}

//----------------------------------------------------------------//
BlockTreeCursor SQLiteBlockTree::AbstractBlockTree_tag ( BlockTreeTag& tag, const BlockTreeTag& otherTag ) {
    
    int nodeID = this->getNodeIDFromTagName ( otherTag.getName ());
    assert ( nodeID != 0 );
    
    this->setTag ( tag.getName (), nodeID );
    return this->findCursorForTag ( tag );
}

//----------------------------------------------------------------//
void SQLiteBlockTree::AbstractBlockTree_update ( shared_ptr < const Block > block ) {

    if ( !block ) return;
    
    string hash     = block->getDigest ().toHex ();
    int blockID     = 0;
    bool exists     = false;
    
    this->mDB.exec (
        "SELECT rowid FROM padamose WHERE hash ID ?1",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, hash );
        },
        
        //--------------------------------//
        [ & ]( int, const SQLiteStatement& stmt ) {
            blockID     = stmt.getValue < int >( 0 );
            exists      = true;
        }
    );
    
    if ( !exists ) return;
    
    this->mDB.exec (
    
        "UPDATE nodes SET block = ?1, hasBlock = 1 WHERE rowid IS ?2",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, ToJSONSerializer::toJSONString ( *block ));
            stmt.bind ( 2, blockID );
        }
    );
    
    this->markRecurse ( blockID, STATUS_COMPLETE );
}

} // namespace Volition
