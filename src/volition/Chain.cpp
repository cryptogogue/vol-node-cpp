// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Chain.h>
#include <volition/ChainMetadata.h>
#include <volition/Format.h>
#include <volition/TheContext.h>

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
int Chain::compare ( const Chain& chain0, const Chain& chain1, time_t now ) {

    size_t height0 = chain0.countBlocks ();
    size_t height1 = chain1.countBlocks ();

    size_t minHeight = height0 < height1 ? height0 : height1;    
    size_t forkHeight = minHeight;
    
    time_t minTime = now;
    
    do {
        forkHeight--;
    
        shared_ptr < Block > block0 = chain0.getBlock ( forkHeight );
        shared_ptr < Block > block1 = chain1.getBlock ( forkHeight );
    
        assert ( block0 && block1 );
    
        if ( *block0 == *block1 ) break;
        
        time_t time0 = block0->getTime ();
        time_t time1 = block1->getTime ();
        
        // TODO: is this safe?
        minTime = time0 < minTime ? time0 : minTime;
        minTime = time1 < minTime ? time1 : minTime;
    }
    while ( forkHeight > 0 );

    assert ( forkHeight < minHeight ); // TODO: handle gracefully
    
    // TODO: maybe use time of top block (instead of current time)
    u64 evalCount = ( difftime ( now, minTime ) / TheContext::get ().getWindow ()) + 1;

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

    string str;

    if ( pre ) {
        Format::write ( str, "%s", pre );
    }

    size_t nBlocks = this->countBlocks ();
    if ( nBlocks ) {
    
        Format::write ( str, "-" );
    
        for ( size_t i = 1; i < nBlocks; ++i ) {
            shared_ptr < Block > block = this->getBlock ( i );
            assert ( block );
            Format::write ( str, ",%s", block->mMinerID.c_str ());
        }
    }

    if ( post ) {
        Format::write ( str, "%s", post );
    }
    
    return str;
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
        shared_ptr < Block > block = Ledger::getObjectOrNull < Block >( chainIt, FormatLedgerKey::forBlock ());
        assert ( block );
        blocks.push_back ( *block );
    }
    serializer.serialize ( "blocks", blocks );
}

} // namespace Volition
