// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Chain.h>
#include <volition/ChainMetadata.h>
#include <volition/Format.h>

namespace Volition {

//================================================================//
// Chain
//================================================================//

//----------------------------------------------------------------//
Chain::Chain () {
}

//----------------------------------------------------------------//
Chain::~Chain () {
}

//----------------------------------------------------------------//
bool Chain::checkMiners ( string miners ) const {
    
    const char* delim = ",";
    
    size_t start;
    size_t end = 0;
    
    size_t blockID = 0;
    while (( start = miners.find_first_not_of ( delim, end )) != std::string::npos ) {
        end = miners.find ( delim, start );
        string minerID = miners.substr ( start, end - start );
        shared_ptr < Block > block = this->getBlock ( blockID );
        if ( !block ) return false;
        if (( blockID > 0 ) && ( block->getMinerID () != minerID )) return false;
        blockID++;
    }
    return true;
}

//----------------------------------------------------------------//
int Chain::compare ( const Chain& chain0, const Chain& chain1, u64 now, u64 window ) {

    size_t height0 = chain0.countBlocks ();
    size_t height1 = chain1.countBlocks ();

    size_t minHeight = height0 < height1 ? height0 : height1;    
    size_t forkHeight = minHeight;
    
    u64 minTime = now;
    
    do {
        forkHeight--;
    
        shared_ptr < Block > block0 = chain0.getBlock ( forkHeight );
        shared_ptr < Block > block1 = chain1.getBlock ( forkHeight );
    
        assert ( block0 && block1 );
    
        if ( *block0 == *block1 ) break;
        
        u64 time0 = block0->getTime ();
        u64 time1 = block1->getTime ();
        
        minTime = time0 < minTime ? time0 : minTime;
        minTime = time1 < minTime ? time1 : minTime;
    }
    while ( forkHeight > 0 );

    assert ( forkHeight < minHeight ); // TODO: handle gracefully
    
    // TODO: mayb use time of top block (instead of current time)
    u64 evalCount = (( now - minTime ) / window ) + 1;

    // only need to compare blocks up through evalCount, starting just after the fork
    int result = Chain::compareSegment ( chain0, chain1, forkHeight + 1, evalCount );
    if ( result == 0 ) {
    
        if ( minHeight < height0 ) {
            result -= ( height0 - minHeight );
        }
    
        if ( minHeight < height1 ) {
            result += ( height1 - minHeight );
        }
    }
    return result;
}

//----------------------------------------------------------------//
int Chain::compareSegment ( const Chain& chain0, const Chain& chain1, size_t base, size_t n ) {

    size_t top = base + n;

    size_t height0 = chain0.countBlocks ();
    size_t height1 = chain1.countBlocks ();

    size_t minHeight = height0 < height1 ? height0 : height1;
    minHeight = minHeight < top ? minHeight : top;

    int score = 0;
    
    for ( size_t i = base; i < minHeight; ++i ) {
    
        shared_ptr < Block > block0 = chain0.getBlock ( i );
        shared_ptr < Block > block1 = chain1.getBlock ( i );
        
        score += Block::compare ( *block0, *block1 );
    }
    return score;
}

//----------------------------------------------------------------//
size_t Chain::countBlocks () const {

    return this->getVersion ();
}

//----------------------------------------------------------------//
string Chain::print ( const char* pre, const char* post ) const {

    return "";

//    string str;
//
//    if ( pre ) {
//        Format::write ( str, "%s", pre );
//    }
//
//    size_t cycleCount = 0;
//    size_t blockCount = 0;
//
//    VersionedStoreIterator chainIt ( *this, 0 );
//
//    shared_ptr < Cycle > prevCycle = Ledger::getJSONSerializableObject < Cycle >( chainIt, CYCLE_KEY );
//    assert ( prevCycle );
//
//    for ( ; chainIt && ( !chainIt.isCurrent ()); chainIt.next ()) {
//        shared_ptr < Cycle > cycle = Ledger::getJSONSerializableObject < Cycle >( chainIt, CYCLE_KEY );
//        assert ( cycle );
//
//        if (( cycleCount == 0 ) || ( cycle->mCycleID != prevCycle->mCycleID )) {
//
//            if ( cycleCount > 0 ) {
//                if ( cycleCount > 1 ) {
//                    Format::write ( str, " (%d)]", ( int )( metaData ? metaData->countParticipants ( cycleCount - 1 ) : blockCount ));
//                }
//                else {
//                    Format::write ( str, "]" );
//                }
//            }
//            Format::write ( str, "[" );
//            cycleCount++;
//            blockCount = 0;
//        }
//
//        shared_ptr < Block > block = Ledger::getJSONSerializableObject < Block >( chainIt, BLOCK_KEY );
//        assert ( block );
//
//        if ( blockCount > 0 ) {
//            Format::write ( str, "," );
//        }
//
//        Format::write ( str, "%s", block->mHeight == 0  ? "." : block->getMinerID ().c_str ());
//        blockCount++;
//        *prevCycle = *cycle;
//    }
//
//    if ( cycleCount > 1 ) {
//        Format::write ( str, " (%d)]", ( int )( metaData ? metaData->countParticipants ( cycleCount - 1 ) : blockCount ));
//    }
//    else {
//        Format::write ( str, "]" );
//    }
//
//    if ( post ) {
//        Format::write ( str, "%s", post );
//    }
//
//    return str;
}

//----------------------------------------------------------------//
bool Chain::pushBlock ( const Block& block ) {

    Chain fork ( *this );

    bool result = block.apply ( fork );

    if ( result ) {
        fork.pushVersion ();
        this->takeSnapshot ( fork );
    }
    return result;
}

//----------------------------------------------------------------//
void Chain::reset () {

    this->Ledger::reset ();
}

//----------------------------------------------------------------//
size_t Chain::size () const {

    return this->VersionedStore::getVersion ();
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Chain::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    this->reset ();
    
    SerializableVector < Block > blocks;
    serializer.serialize ( "blocks", blocks );

    size_t size = blocks.size ();
    for ( size_t i = 0; i < size; ++i ) {
    
        Block block = blocks [ i ];
        this->pushBlock ( block );
    }
}

//----------------------------------------------------------------//
void Chain::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    SerializableVector < Block > blocks;
    
    size_t top = this->getVersion ();
    VersionedStoreIterator chainIt ( *this, 0 );
    for ( ; chainIt && ( chainIt.getVersion () < top ); chainIt.next ()) {
        shared_ptr < Block > block = Ledger::getJSONSerializableObject < Block >( chainIt, BLOCK_KEY );
        assert ( block );
        blocks.push_back ( *block );
    }
    serializer.serialize ( "blocks", blocks );
}

} // namespace Volition
