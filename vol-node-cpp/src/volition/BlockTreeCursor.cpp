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
    mStatus ( STATUS_INVALID ),
    mMeta ( META_NONE ) {
}

//----------------------------------------------------------------//
BlockTreeCursor::~BlockTreeCursor () {
}

//----------------------------------------------------------------//
bool BlockTreeCursor::checkStatus ( kBlockTreeEntryStatus status ) const {

    return ( this->mStatus & status );
}

//----------------------------------------------------------------//
bool BlockTreeCursor::checkTree ( const AbstractBlockTree* tree ) const {

    return (( this->mTree == NULL) || ( this->mTree == tree ));
}

//----------------------------------------------------------------//
int BlockTreeCursor::compare ( const BlockTreeCursor& node0, const BlockTreeCursor& node1, kRewriteMode rewriteMode ) {

    assert ( node0.mTree == node1.mTree );
    assert ( node0.mTree );
    
    return node0.mTree->compare ( node0, node1, rewriteMode );
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

    return this->mBlock;
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
kBlockTreeEntryStatus BlockTreeCursor::getStatus () const {
    
    return this->mStatus;
}

//----------------------------------------------------------------//
bool BlockTreeCursor::hasHeader () const {
    return ( this->mHeader || this->mBlock );
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

    return ( this->mStatus & STATUS_COMPLETE );
}

//----------------------------------------------------------------//
bool BlockTreeCursor::isInvalid () const {

    return ( this->mStatus & STATUS_INVALID );
}

//----------------------------------------------------------------//
bool BlockTreeCursor::isMissing () const {

    return ( this->mStatus & STATUS_MISSING );
}

//----------------------------------------------------------------//
bool BlockTreeCursor::isMissingOrInvalid () const {

    return ( this->mStatus & ( STATUS_MISSING | STATUS_INVALID ));
}

//----------------------------------------------------------------//
bool BlockTreeCursor::isNew () const {

    return ( this->mStatus & STATUS_NEW );
}

//----------------------------------------------------------------//
bool BlockTreeCursor::isRefused () const {

    return ( this->mMeta == META_REFUSED );
}

//----------------------------------------------------------------//
void BlockTreeCursor::logBranchRecurse ( string& str ) const {

    BlockTreeCursor parent = this->getParent ();
    if ( parent.hasHeader ()) {
        parent.logBranchRecurse ( str );
    }
    const BlockHeader& header = *this->mHeader;
    
    cc8* status = "";
    
    if ( this->mMeta != META_NONE ) {
    
        switch ( this->mMeta ) {
            
            case META_PROVISIONAL:
                status = "*";
                break;
            
            case META_REFUSED:
                status = "#";
                break;
            
            default:
                break;
        }
    }
    else {
    
        switch ( this->mStatus ) {
            
            case STATUS_NEW:
                status = "N";
                break;
                
            case STATUS_COMPLETE:
                status = "C";
                break;
            
            case STATUS_MISSING:
                status = "?";
                break;
                
            case STATUS_INVALID:
                status = "X";
                break;
        }
    }
    
    string charm = this->getCharmTag ();
    cc8* format = this->mBlock ? "%s%d [%s:%s:%s]" : "%s%d <%s:%s:%s>";
    
    size_t height = header.getHeight ();
    Format::write ( str, format, parent.hasHeader () ? ", " : "", ( int )height, ( height > 0 ) ? header.getMinerID ().c_str () : "-", charm.c_str (), status );
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeCursor::trim ( kBlockTreeEntryStatus status ) const {

    BlockTreeCursor cursor = *this;

    while ( cursor.hasHeader () && ( cursor.mStatus & status )) {
        cursor = cursor.getParent ();
    }
    return cursor;
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeCursor::trimInvalid () const {

    return this->trim ( STATUS_INVALID );
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeCursor::trimMissing () const {

    return this->trim ( STATUS_MISSING );
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeCursor::trimMissingOrInvalid () const {

    return this->trim (( kBlockTreeEntryStatus )( STATUS_MISSING | STATUS_INVALID ));
}

//----------------------------------------------------------------//
string BlockTreeCursor::writeBranch () const {

    string str;
    this->logBranchRecurse ( str );
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
