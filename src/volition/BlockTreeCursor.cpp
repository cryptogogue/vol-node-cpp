// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractBlockTree.h>
#include <volition/Block.h>
#include <volition/BlockTreeCursor.h>

// To compare chains:
// 1. Find the common root.
// 2. Get the interval between the timestamp of the end of the longest branch and its first block (excluding the root).
//      a. From the interval, divide by the lookback window to calculate COMPARE_COUNT.
// 3. From the common root, up to the COMPARE_COUNT, compare each block and tally the score for each chain.
//      a. +1 for the winner, -1 to the loser; 0 if tied.
// 4. Select the winner.
//      a. The chain with the highest score wins.
//      b. If chains are tied and the same length, pick the chain with the best ending block.
//      c. If chains are tied and different length, extend the shorter chain by one as a tie-breaker.

namespace Volition {

//================================================================//
// BlockTreeCursor
//================================================================//

//----------------------------------------------------------------//
BlockTreeCursor::BlockTreeCursor () :
    mTree ( NULL ),
    mBranchStatus ( BRANCH_STATUS_NEW ),
    mSearchStatus ( SEARCH_STATUS_NEW ) {
}

//----------------------------------------------------------------//
BlockTreeCursor::~BlockTreeCursor () {
}

//----------------------------------------------------------------//
bool BlockTreeCursor::checkTree ( const AbstractBlockTree* tree ) const {

    return (( this->mTree == NULL) || ( this->mTree == tree ));
}

//----------------------------------------------------------------//
int BlockTreeCursor::compare ( const BlockTreeCursor& node0, const BlockTreeCursor& node1 ) {

    assert ( node0.mTree == node1.mTree );
    assert ( node0.mTree );
    
    return node0.mTree->compare ( node0, node1 );
}

//----------------------------------------------------------------//
bool BlockTreeCursor::equals ( const BlockTreeCursor& rhs ) const {
    
    return ( this->mHeader && rhs.mHeader ) ? ( this->mHeader->equals ( *rhs.mHeader )) : ( this->mHeader == rhs.mHeader );
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeCursor::findRoot ( const BlockTreeCursor& node0, const BlockTreeCursor& node1 ) {

    assert ( node0.mTree == node1.mTree );
    assert ( node0.mTree );

    return node0.mTree->findRoot ( node0, node1 );
}

//----------------------------------------------------------------//
shared_ptr < const Block > BlockTreeCursor::getBlock () const {

    return ( this->hasBlock () && this->mTree ) ? this->mTree->getBlock ( *this ) : NULL;
}

//----------------------------------------------------------------//
string BlockTreeCursor::getHash () const {
    return this->mHeader ? this->mHeader->getDigest ().toHex () : "";
}

//----------------------------------------------------------------//
const BlockHeader& BlockTreeCursor::getHeader () const {

    assert ( this->mHeader );
    return *this->mHeader;
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeCursor::getParent () const {

    return this->mTree ? this->mTree->getParent ( *this ) : BlockTreeCursor ();
}

//----------------------------------------------------------------//
bool BlockTreeCursor::hasBlock () const {
    return ( this->mSearchStatus == SEARCH_STATUS_HAS_BLOCK );
}

//----------------------------------------------------------------//
bool BlockTreeCursor::hasHeader () const {
    return ( bool )this->mHeader;
}

//----------------------------------------------------------------//
bool BlockTreeCursor::hasParent () const {

    return this->mTree ? this->mTree->getParent ( *this ).hasHeader () : false;
}

//----------------------------------------------------------------//
bool BlockTreeCursor::isAncestorOf ( BlockTreeCursor tail ) const {

    assert ( this->mHeader );
    assert ( tail.mHeader );
    
    while ( tail.getHeight () > this->getHeight ()) {
        tail = tail.getParent ();
    }
    return ( this->mHeader->equals ( *tail.mHeader ));
}

//----------------------------------------------------------------//
bool BlockTreeCursor::isComplete () const {

    return ( this->mBranchStatus & BRANCH_STATUS_COMPLETE );
}

//----------------------------------------------------------------//
bool BlockTreeCursor::isInvalid () const {

    return ( this->mBranchStatus & BRANCH_STATUS_INVALID );
}

//----------------------------------------------------------------//
bool BlockTreeCursor::isMissing () const {

    return ( this->mBranchStatus & BRANCH_STATUS_MISSING );
}

//----------------------------------------------------------------//
bool BlockTreeCursor::isMissingOrInvalid () const {

    return ( this->mBranchStatus & ( BRANCH_STATUS_MISSING | BRANCH_STATUS_INVALID ));
}

//----------------------------------------------------------------//
bool BlockTreeCursor::isNew () const {

    return ( this->mBranchStatus & BRANCH_STATUS_NEW );
}

//----------------------------------------------------------------//
bool BlockTreeCursor::isProvisional () const {

    return ( this->mSearchStatus == SEARCH_STATUS_PROVISIONAL );
}

//----------------------------------------------------------------//
void BlockTreeCursor::log ( string& str, string prefix ) const {
    
    cc8* status = "";
    
    if ( this->mSearchStatus == SEARCH_STATUS_PROVISIONAL ) {
    
        status = "*";
    }
    else {

        switch ( this->mBranchStatus ) {
            
            case BRANCH_STATUS_NEW:
                status = "N";
                break;
                
            case BRANCH_STATUS_COMPLETE:
                status = "C";
                break;
            
            case BRANCH_STATUS_MISSING:
                status = "?";
                break;
                
            case BRANCH_STATUS_INVALID:
                status = "C";
                break;
        }
    }
    
    string charm = this->getCharmTag ();
    cc8* format = this->hasBlock () ? "%s%d [%s:%s:%s]" : "%s%d <%s:%s:%s>";
    
    const BlockHeader& header = *this->mHeader;
    size_t height = header.getHeight ();
    Format::write ( str, format, prefix.c_str (), ( int )height, ( height > 0 ) ? header.getMinerID ().c_str () : "-", charm.c_str (), status );
}

//----------------------------------------------------------------//
void BlockTreeCursor::logBranchRecurse ( string& str, size_t maxDepth ) const {

    if ( !maxDepth ) return;

    BlockTreeCursor parent = this->getParent ();
    if ( parent.hasHeader ()) {
        parent.logBranchRecurse ( str, maxDepth - 1 );
    }
    this->log ( str, parent.hasHeader () ? ", " : "" );
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeCursor::trimBranch ( kBlockTreeBranchStatus statusMask ) const {

    BlockTreeCursor cursor = *this;

    while ( cursor.hasHeader () && ( cursor.mBranchStatus & statusMask )) {
        cursor = cursor.getParent ();
    }
    return cursor;
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeCursor::trimInvalidBranch () const {

    return this->trimBranch ( BRANCH_STATUS_INVALID );
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeCursor::trimMissingBranch () const {

    return this->trimBranch ( BRANCH_STATUS_MISSING );
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeCursor::trimMissingOrInvalidBranch () const {

    return this->trimBranch (( kBlockTreeBranchStatus )( BRANCH_STATUS_MISSING | BRANCH_STATUS_INVALID ));
}

//----------------------------------------------------------------//
string BlockTreeCursor::write () const {

    string str;
    this->log ( str );
    return str;
}

//----------------------------------------------------------------//
string BlockTreeCursor::writeBranch ( size_t maxDepth ) const {

    string str;
    this->logBranchRecurse ( str, maxDepth );
    return str;
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
const BlockHeaderFields& BlockTreeCursor::HasBlockHeader_getFields () const {

    return this->getHeader ().getFields ();
}

} // namespace Volition
