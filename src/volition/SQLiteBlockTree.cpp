// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/SQLiteBlockTree.h>

#define SQL_STR(...) #__VA_ARGS__

namespace Volition {

//================================================================//
// BlockCursorCache
//================================================================//

//----------------------------------------------------------------//
BlockCursorCache::BlockCursorCache () :
    mMaxSize ( 0 ) {
}

//----------------------------------------------------------------//
BlockCursorCache::~BlockCursorCache () {
}

//----------------------------------------------------------------//
void BlockCursorCache::cacheCursor ( int nodeID, const BlockTreeCursor& cursor ) {

    if ( !this->mMaxSize ) return;

    // if block is already in the cache, update it
    if ( this->mCache.find ( nodeID ) != this->mCache.cend ()) {

        // erase the original key
        assert ( this->mCacheKeysByNodeID.find ( nodeID ) != this->mCacheKeysByNodeID.cend ());
        BlockCursorCacheKey lastKey = this->mCacheKeysByNodeID [ nodeID ];
        this->mExpirationSet.erase ( lastKey );
        this->mCacheKeysByNodeID.erase ( nodeID );
    }
    else {
    
        // make room
        while ( this->mCache.size () > ( this->mMaxSize - 1 )) {
            
            set < BlockCursorCacheKey >::reverse_iterator expiryIt = this->mExpirationSet.rbegin ();
            
            map < int, BlockTreeCursor >::iterator cacheIt = this->mCache.find ( expiryIt->mNodeID );
            assert ( cacheIt != this->mCache.end ());
            
            this->mCacheKeysByNodeID.erase ( cacheIt->first ); // do this first
            this->mNodeIDByHash.erase ( cacheIt->second.getHash ()); // do this first
            
            this->mExpirationSet.erase ( *expiryIt ); // now it's safe - will invalidate iterator
            this->mCache.erase ( cacheIt ); // now it's safe - will invalidate iterator
            
            assert ( this->mCacheKeysByNodeID.size () == this->mCache.size ());
            assert ( this->mExpirationSet.size () == this->mCache.size ());
            assert ( this->mNodeIDByHash.size () == this->mCache.size ());
        }
    }
    
    BlockCursorCacheKey key = BlockCursorCacheKey ( nodeID );
    this->mCacheKeysByNodeID [ nodeID ] = key;
    this->mExpirationSet.insert ( key );
    this->mNodeIDByHash [ cursor.getHash ()] = nodeID;
    this->mCache [ nodeID ] = cursor;
}

//----------------------------------------------------------------//
const BlockTreeCursor* BlockCursorCache::getCursor ( int nodeID ) const {

    map < int, BlockTreeCursor >::const_iterator cursorIt = this->mCache.find ( nodeID );
    if ( cursorIt != this->mCache.cend ()) {
        return &cursorIt->second;
    }
    return NULL;
}

//----------------------------------------------------------------//
int BlockCursorCache::getNodeIDFromHash ( string hash ) const {

    map < string, int >::const_iterator cursorIt = this->mNodeIDByHash.find ( hash );
    if ( cursorIt != this->mNodeIDByHash.cend ()) {
        return cursorIt->second;
    }
    return 0;
}

//----------------------------------------------------------------//
void BlockCursorCache::invalidate ( int nodeID ) {

    map < int, BlockTreeCursor >::iterator cursorIt = this->mCache.find ( nodeID );
    if ( cursorIt != this->mCache.cend ()) {

        // erase the original key
        assert ( this->mCacheKeysByNodeID.find ( nodeID ) != this->mCacheKeysByNodeID.cend ());
        BlockCursorCacheKey lastKey = this->mCacheKeysByNodeID [ nodeID ];
        this->mExpirationSet.erase ( lastKey );
        this->mCacheKeysByNodeID.erase ( nodeID );
        this->mNodeIDByHash.erase ( cursorIt->second.getHash ());
        this->mCache.erase ( nodeID );
    }
}

//----------------------------------------------------------------//
void BlockCursorCache::setMaxSize ( size_t size ) {

    this->mMaxSize = size;
}

//================================================================//
// SQLiteBlockTree
//================================================================//

//----------------------------------------------------------------//
int SQLiteBlockTree::getNodeIDFromHash ( string hash ) const {

    int nodeID = this->mCache.getNodeIDFromHash ( hash );
    if ( nodeID ) return nodeID;
    
    SQLiteResult result = this->mDB.exec (
        
        "SELECT nodeID FROM nodes WHERE hash IS ?1",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, hash );
        },
        
        //--------------------------------//
        [ & ]( int, const SQLiteStatement& stmt ) {
            nodeID = stmt.getValue < int >( 0 );
        }
    );
    result.reportWithAssert ();
    
    return nodeID;
}

//----------------------------------------------------------------//
int SQLiteBlockTree::getNodeIDFromTagName ( string tagName ) const {

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
    result.reportWithAssert ();
    
    return nodeID;
}

//----------------------------------------------------------------//
kBlockTreeBranchStatus SQLiteBlockTree::getNodeBranchStatus ( int nodeID, kBlockTreeBranchStatus status ) const {
            
    SQLiteResult result = this->mDB.exec (
        
        "SELECT branchStatus FROM nodes WHERE nodeID IS ?1",
        
        //--------------------------------//
        [ & ]( SQLiteStatement stmt ) {
            stmt.bind ( 1, nodeID );
        },
        
        //--------------------------------//
        [ & ]( int, SQLiteStatement stmt ) {
            status = stringToBranchStatus ( stmt.getValue < string >( 0 ));
        }
    );
    result.reportWithAssert ();
    
    return status;
}

//----------------------------------------------------------------//
void SQLiteBlockTree::pruneUnreferencedNodes () {
    
    // TODO: this was killing performance with nontrivial trees; optimize or find another way
    
    // take a look at this gem! with a union including a self-join.
    // should delete all nodes with no references (other nodes or tags).
    
//    SQLiteResult result = this->mDB.exec ( SQL_STR (
//        DELETE FROM nodes
//        WHERE nodeID NOT IN (
//            SELECT nodeID FROM tags WHERE nodeID IS nodes.nodeID
//            UNION
//            SELECT a.nodeID FROM nodes a INNER JOIN nodes b ON a.nodeID IS b.parentID
//        )
//    ));
//    assert ( result );
}

//----------------------------------------------------------------//
BlockTreeCursor SQLiteBlockTree::readCursor ( const SQLiteStatement& stmt ) const {

    string headerJSON       = stmt.getValue < string >( "header" );
    string branchStatus     = stmt.getValue < string >( "branchStatus" );
    string searchStatus     = stmt.getValue < string >( "searchStatus" );

    // header *must* exist.
    assert ( headerJSON.size ());
    
    shared_ptr < BlockHeader >header = make_shared < BlockHeader >();
    FromJSONSerializer::fromJSONString ( *header, headerJSON );

    return this->makeCursor (
        header,
        stringToBranchStatus ( branchStatus ),
        stringToSearchStatus ( searchStatus )
    );
}

//----------------------------------------------------------------//
void SQLiteBlockTree::setBranchStatus ( int nodeID, const Digest& parentDigest, kBlockTreeBranchStatus status ) {

    if ( !nodeID ) return;

    if ( status == BRANCH_STATUS_COMPLETE ) {

        int parentID = this->getNodeIDFromHash ( parentDigest.toHex ());

        if ( parentID ) {
            kBlockTreeBranchStatus parentBranchStatus = this->getNodeBranchStatus ( parentID, kBlockTreeBranchStatus::BRANCH_STATUS_INVALID );
            if ( parentBranchStatus != BRANCH_STATUS_COMPLETE ) return;
        }
    }
    
    set < int > queue;
    queue.insert ( nodeID );
    
    if ( queue.size ()) {
        
        this->mDB.beginTransaction ();
    
        while ( queue.size ()) {
            
            set < int >::iterator queueIt = queue.begin ();
            nodeID = *queueIt;
            queue.erase ( queueIt );
            
            this->setBranchStatusInner ( nodeID, status, queue );
        }
        
        this->mDB.commitTransaction ();
    }
}

//----------------------------------------------------------------//
void SQLiteBlockTree::setBranchStatusInner ( int nodeID, kBlockTreeBranchStatus status, set < int >& queue ) {

    SQLiteResult result;

    kBlockTreeBranchStatus prevBranchStatus;
    kBlockTreeSearchStatus searchStatus;
    bool exists = false;

    // first, get some information about the node as it exists now.
    result = this->mDB.exec (

        "SELECT branchStatus, searchStatus FROM nodes WHERE nodeID IS ?1",

        //--------------------------------//
        [ & ]( SQLiteStatement stmt ) {
            stmt.bind ( 1, nodeID );
        },

        //--------------------------------//
        [ & ]( int, SQLiteStatement stmt ) {
            prevBranchStatus    = SQLiteBlockTree::stringToBranchStatus ( stmt.getValue < string >( 0 ));
            searchStatus        = SQLiteBlockTree::stringToSearchStatus ( stmt.getValue < string >( 1 ));
            exists              = true;
        }
    );
    result.reportWithAssert ();

    // block doesn't exist.
    if ( !exists ) return;
    
    // status already matches; nothing else to do.
    if ( prevBranchStatus == status ) return;

    // cannot recover an invalid branch.
    if ( prevBranchStatus == BRANCH_STATUS_INVALID ) return;

    // cannot complete without a block.
    if (( status == BRANCH_STATUS_COMPLETE ) && ( searchStatus != SEARCH_STATUS_HAS_BLOCK )) return;

    this->mCache.invalidate ( nodeID );

    result = this->mDB.exec (

        "UPDATE nodes SET branchStatus = ?1 WHERE nodeID IS ?2",

        //--------------------------------//
        [ & ]( SQLiteStatement stmt ) {
            stmt.bind ( 1,  SQLiteBlockTree::stringFromBranchStatus ( status ) );
            stmt.bind ( 2,  nodeID );
        }
    );
    result.reportWithAssert ();

    // get the child node (if any).
    result = this->mDB.exec (

        "SELECT nodeID FROM nodes WHERE parentID IS ?1",

        //--------------------------------//
        [ & ]( SQLiteStatement stmt ) {
            stmt.bind ( 1, nodeID );
        },

        //--------------------------------//
        [ & ]( int, SQLiteStatement stmt ) {
            queue.insert ( stmt.getValue < int >( 0 ));
        }
    );
    result.reportWithAssert ();
}

//----------------------------------------------------------------//
void SQLiteBlockTree::setSearchStatus ( int nodeID, kBlockTreeSearchStatus status ) {

    if ( !nodeID ) return;
    
    this->mCache.invalidate ( nodeID );
    
    // go ahead and update the status.
    SQLiteResult result = this->mDB.exec (

        "UPDATE nodes SET searchStatus = ?1 WHERE nodeID IS ?2",

        //--------------------------------//
        [ & ]( SQLiteStatement stmt ) {
            stmt.bind ( 1,  SQLiteBlockTree::stringFromSearchStatus ( status ));
            stmt.bind ( 2,  nodeID );
        }
    );
    result.reportWithAssert ();
}

//----------------------------------------------------------------//
void SQLiteBlockTree::setTag ( string tagName, int nodeID ) {

    if ( !nodeID ) return;

    int prevNodeID = this->getNodeIDFromTagName ( tagName );
    
    if ( prevNodeID == nodeID ) return;

    SQLiteResult result = this->mDB.exec (
        
        "INSERT INTO tags ( name, nodeID ) VALUES ( ?1, ?2 ) ON CONFLICT ( name ) DO UPDATE SET nodeID = ?2",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, tagName );
            stmt.bind ( 2, nodeID );
        }
    );
    result.reportWithAssert ();
    
    this->pruneUnreferencedNodes ();
}

//----------------------------------------------------------------//
string SQLiteBlockTree::stringFromBranchStatus ( kBlockTreeBranchStatus status ) {

    switch ( status ) {
        case kBlockTreeBranchStatus::BRANCH_STATUS_NEW:             return "N";
        case kBlockTreeBranchStatus::BRANCH_STATUS_COMPLETE:        return "C";
        case kBlockTreeBranchStatus::BRANCH_STATUS_MISSING:         return "?";
        case kBlockTreeBranchStatus::BRANCH_STATUS_INVALID:         return "X";
    }
    return "";
}

//----------------------------------------------------------------//
string SQLiteBlockTree::stringFromSearchStatus ( kBlockTreeSearchStatus status ) {

    switch ( status ) {
        case kBlockTreeSearchStatus::SEARCH_STATUS_NEW:             return "+";
        case kBlockTreeSearchStatus::SEARCH_STATUS_PROVISIONAL:     return "*";
        case kBlockTreeSearchStatus::SEARCH_STATUS_HAS_BLOCK:       return "#";
    }
    return "";
}

//----------------------------------------------------------------//
kBlockTreeBranchStatus SQLiteBlockTree::stringToBranchStatus ( string str ) {

    char c = str.size () ? str [ 0 ] : 0;

    switch ( c ) {
        case 'N':       return kBlockTreeBranchStatus::BRANCH_STATUS_NEW;
        case 'C':       return kBlockTreeBranchStatus::BRANCH_STATUS_COMPLETE;
        case '?':       return kBlockTreeBranchStatus::BRANCH_STATUS_MISSING;
        case 'X':       return kBlockTreeBranchStatus::BRANCH_STATUS_INVALID;
    }
    assert ( false );
    return kBlockTreeBranchStatus::BRANCH_STATUS_INVALID;
}

//----------------------------------------------------------------//
kBlockTreeSearchStatus SQLiteBlockTree::stringToSearchStatus ( string str ) {

    char c = str.size () ? str [ 0 ] : 0;

    switch ( c ) {
        case '~':       // leftover from SEARCH_STATUS_SEARCHING
        case '+':       return kBlockTreeSearchStatus::SEARCH_STATUS_NEW;
        case '*':       return kBlockTreeSearchStatus::SEARCH_STATUS_PROVISIONAL;
        case '#':       return kBlockTreeSearchStatus::SEARCH_STATUS_HAS_BLOCK;
    }
    assert ( false );
    return kBlockTreeSearchStatus::SEARCH_STATUS_NEW;
}

//----------------------------------------------------------------//
SQLiteBlockTree::SQLiteBlockTree ( string filename, SQLiteConfig config ) {

    SQLiteResult result = this->mDB.open ( filename, config );
    result.reportWithAssert ();
    
    size_t userVersion = 0;
    result = this->mDB.exec ( "PRAGMA user_version", NULL,

        //--------------------------------//
        [ & ]( int, const SQLiteStatement& stmt ) {
            userVersion = ( size_t ) stmt.getValue < int >( 0 );
        }
    );
    result.reportWithAssert ();

    if ( userVersion && ( userVersion < MIN_SUPPORTED_USER_VERSION )) throw SQLiteBlockTreeUnsupportedVersionException ();
    
    // nodes
    result = this->mDB.exec ( SQL_STR (
        CREATE TABLE IF NOT EXISTS nodes (
            nodeID          INTEGER                                                 PRIMARY KEY,
            parentID        INTEGER                                                 NOT NULL DEFAULT 0,
            hash            TEXT                                                    NOT NULL,
            height          INTEGER                                                 NOT NULL DEFAULT 0,
            header          TEXT                                                    NOT NULL,
            block           TEXT,
            
            branchStatus    TEXT CHECK ( branchStatus IN ( 'N', 'C', '?', 'X' ))    NOT NULL DEFAULT 'N',
            searchStatus    TEXT CHECK ( searchStatus IN ( '+', '*', '~', '#' ))    NOT NULL DEFAULT '+',
            
            FOREIGN KEY ( parentID ) REFERENCES nodes ( nodeID )
        )
    ));
    result.reportWithAssert ();
    
    // tags
    result = this->mDB.exec ( SQL_STR (
        CREATE TABLE IF NOT EXISTS tags (
            tagID       INTEGER                 PRIMARY KEY,
            nodeID      INTEGER                 NOT NULL,
            name        TEXT                    NOT NULL,
            FOREIGN KEY ( nodeID ) REFERENCES nodes ( nodeID )
        )
    ));
    result.reportWithAssert ();
    
    // indices
    
    result = this->mDB.exec ( SQL_STR ( CREATE UNIQUE INDEX IF NOT EXISTS hash ON nodes ( hash )));
    result.reportWithAssert ();
    
    result = this->mDB.exec ( SQL_STR ( CREATE INDEX IF NOT EXISTS parentID ON nodes ( parentID )));
    result.reportWithAssert ();
    
    result = this->mDB.exec ( SQL_STR ( CREATE UNIQUE INDEX IF NOT EXISTS name ON tags ( name )));
    result.reportWithAssert ();
    
    if ( userVersion != CURRENT_USER_VERSION ) {
        result = this->mDB.exec ( Format::write ( "PRAGMA user_version = %d", ( int )CURRENT_USER_VERSION ));
        result.reportWithAssert ();
    }
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
        return this->AbstractBlockTree_tag ( tag, cursor );
    }

        
    kBlockTreeSearchStatus searchStatus = isProvisional ? kBlockTreeSearchStatus::SEARCH_STATUS_PROVISIONAL : kBlockTreeSearchStatus::SEARCH_STATUS_NEW;
    searchStatus = block ? kBlockTreeSearchStatus::SEARCH_STATUS_HAS_BLOCK : searchStatus;

    kBlockTreeBranchStatus branchStatus = block ? kBlockTreeBranchStatus::BRANCH_STATUS_COMPLETE : kBlockTreeBranchStatus::BRANCH_STATUS_NEW;

    int parentID = header->isGenesis () ? 0 : this->getNodeIDFromHash ( header->getPrevDigest ().toHex ());

    if ( parentID ) {
        
        branchStatus = this->getNodeBranchStatus ( parentID, kBlockTreeBranchStatus::BRANCH_STATUS_INVALID );
        
        if (( branchStatus == BRANCH_STATUS_COMPLETE ) && ( !block )) {
            branchStatus = BRANCH_STATUS_NEW;
        }
    }

    // insert node
    SQLiteResult result = this->mDB.exec (
        
        "INSERT INTO nodes ( parentID, hash, height, header, block, branchStatus, searchStatus ) VALUES ( ?1, ?2, ?3, ?4, ?5, ?6, ?7 )",
        
        //--------------------------------//
        [ & ]( SQLiteStatement stmt ) {
            
            BlockHeader headerOnly = *header;
            
            stmt.bind ( 1,      parentID );
            stmt.bind ( 2,      hash );
            stmt.bind ( 3,      ( int )header->getHeight ());
            stmt.bind ( 4,      ToJSONSerializer::toJSONString ( headerOnly ));
            stmt.bind ( 5,      block ? ToJSONSerializer::toJSONString ( *block ) : "" );
            stmt.bind ( 6,      SQLiteBlockTree::stringFromBranchStatus ( branchStatus ));
            stmt.bind ( 7,      SQLiteBlockTree::stringFromSearchStatus ( searchStatus ));
        }
    );
    result.reportWithAssert ();
    
    int nodeID = ( int )sqlite3_last_insert_rowid ( this->mDB );
    this->setTag ( tagName, nodeID );

    // make the cursor
    cursor = this->makeCursor ( header, branchStatus, searchStatus );
    return cursor;
}

//----------------------------------------------------------------//
BlockTreeCursor SQLiteBlockTree::AbstractBlockTree_findCursorForHash ( string hash ) const {

    BlockTreeCursor cursor;
    
    int nodeID = this->getNodeIDFromHash ( hash );
    if ( !nodeID ) return cursor;

    const BlockTreeCursor* cursorFromCache = this->mCache.getCursor ( nodeID );
    if ( cursorFromCache ) return *cursorFromCache;

    SQLiteResult result = this->mDB.exec (
        
        "SELECT hash, header, branchStatus, searchStatus FROM nodes WHERE nodeID IS ?1",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, nodeID );
        },
        
        //--------------------------------//
        [ & ]( int, const SQLiteStatement& stmt ) {
            cursor = this->readCursor ( stmt );
        }
    );
    result.reportWithAssert ();

    this->mCache.cacheCursor ( nodeID, cursor );

    return cursor;
}

//----------------------------------------------------------------//
BlockTreeCursor SQLiteBlockTree::AbstractBlockTree_findCursorForTagName ( string tagName ) const {

    BlockTreeCursor cursor;

    SQLiteResult result = this->mDB.exec (
        
        "SELECT header, branchStatus, searchStatus FROM nodes INNER JOIN tags ON tags.nodeID = nodes.nodeID WHERE tags.name IS ?1",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, tagName );
        },
        
        //--------------------------------//
        [ & ]( int, const SQLiteStatement& stmt ) {
            cursor = this->readCursor ( stmt );
        }
    );
    result.reportWithAssert ();

    return cursor;
}

//----------------------------------------------------------------//
shared_ptr < const Block > SQLiteBlockTree::AbstractBlockTree_getBlock ( const BlockTreeCursor& cursor ) const {

    assert ( cursor.getTree () == this );
    if ( !cursor.hasBlock ()) return NULL;

    shared_ptr < Block > block;

    SQLiteResult result = this->mDB.exec (
        
        "SELECT block FROM nodes WHERE hash IS ?1",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, cursor.getHash ());
        },
        
        //--------------------------------//
        [ & ]( int, const SQLiteStatement& stmt ) {
        
            string blockJSON = stmt.getValue < string >( "block" );

            assert ( blockJSON.size ());
        
            block = make_shared < Block >();
            FromJSONSerializer::fromJSONString ( *block, blockJSON );
        }
    );
    result.reportWithAssert ();

    assert ( block );
    return block;
}

//----------------------------------------------------------------//
void SQLiteBlockTree::AbstractBlockTree_setBranchStatus ( const BlockTreeCursor& cursor, kBlockTreeBranchStatus status ) {

    int nodeID = this->getNodeIDFromHash ( cursor.getDigest ().toHex ());
    this->setBranchStatus ( nodeID, cursor.getPrevDigest (), status );
}

//----------------------------------------------------------------//
void SQLiteBlockTree::AbstractBlockTree_setCacheSize ( size_t size ) {
    this->mCache.setMaxSize ( size );
}

//----------------------------------------------------------------//
void SQLiteBlockTree::AbstractBlockTree_setSearchStatus ( const BlockTreeCursor& cursor, kBlockTreeSearchStatus status ) {

    int nodeID = this->getNodeIDFromHash ( cursor.getDigest ().toHex ());
    this->setSearchStatus ( nodeID, status );
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
    
    int nodeID = this->getNodeIDFromHash ( block->getDigest ().toHex ());
    if ( !nodeID ) return;
    
    this->mCache.invalidate ( nodeID );
    
    SQLiteResult result;
        
    this->mDB.beginTransaction ();
    
    result = this->mDB.exec (
    
        "UPDATE nodes SET block = ?1, searchStatus = '#' WHERE nodeID IS ?2",
        
        //--------------------------------//
        [ & ]( SQLiteStatement& stmt ) {
            stmt.bind ( 1, ToJSONSerializer::toJSONString ( *block ));
            stmt.bind ( 2, nodeID );
        }
    );
    result.reportWithAssert ();
    
    this->setBranchStatus ( nodeID, block->getPrevDigest (), BRANCH_STATUS_COMPLETE );

    this->mDB.commitTransaction ();
}

} // namespace Volition
