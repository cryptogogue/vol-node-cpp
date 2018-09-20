// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Chain.h>
#include <volition/ChainMetadata.h>
#include <volition/Cycle.h>
#include <volition/VersionedStoreIterator.h>
#include <volition/VersionedValueIterator.h>

namespace Volition {

static const double THRESHOLD = 0.75;

//================================================================//
// Chain
//================================================================//

//----------------------------------------------------------------//
bool Chain::canEdit ( const Cycle& cycle0, const Cycle& cycle1, string minerID ) {

    assert ( cycle0.mCycleID == ( cycle1.mCycleID - 1 )); // don't send us cycles out of order!

    if ( cycle0.mCycleID == 0 ) return false; // cannot change genesis cycle

    // cycle is neither last nor genesis; can change if majority of miners have not yet
    // agreed on next cycle

    size_t size0            = this->countParticipants ( cycle0, minerID );
    size_t size1            = this->countParticipants ( cycle1 );

    float ratio             = ( float )size1 / ( float )size0;

    return ratio <= THRESHOLD;
}

//----------------------------------------------------------------//
bool Chain::canEdit ( size_t cycleID, const Chain& chain ) const {

//    if ( this->canEdit ( cycleID )) return true;
//
//    size_t participants0 = this->mCycles [ cycleID ]->countMiners ();
//    size_t participants1 = chain.mCycles [ cycleID ]->countMiners ();
//
//    if ( participants0 < participants1 ) {
//
//        float ratio = ( float )participants0 / ( float )participants1;
//        if ( ratio <= THRESHOLD ) return true;
//    }
//
//    size_t size0 = this->mCycles.size ();
//    size_t size1 = chain.mCycles.size ();
//
//    size_t min = ( size0 < size1 ) ? size0 : size1;
//
//    for ( size_t i = cycleID + 1; i < ( min - 1 ); ++i ) {
//        if ( this->mCycles [ i ]->countMiners () < chain.mCycles [ i ]->countMiners ()) {
//            return true;
//        }
//    }
//    return false;

    return false;
}

//----------------------------------------------------------------//
bool Chain::canPush ( string minerID, bool force ) {

    return ( force || ( !this->isInCycle ( this->getTopCycle (), minerID )));
}

//----------------------------------------------------------------//
Chain::Chain () {
}

//----------------------------------------------------------------//
Chain::Chain ( Block& genesisBlock ) {

    this->mMetaData = make_shared < ChainMetadata >();

    this->newCycle ();
    this->pushBlock ( genesisBlock );
    assert ( this->getValueOrNil < Block >( BLOCK_KEY ));
}

//----------------------------------------------------------------//
Chain::Chain ( const Chain& chain ) {

//    size_t nCycles = chain.mCycles.size ();
//
//    this->mCycles.reserve ( nCycles );
//    for ( size_t i = 0; i < nCycles; ++i ) {
//        this->mCycles.push_back ( make_unique < Cycle >( *chain.mCycles [ i ]));
//    }
//    this->rebuildState ();
}

//----------------------------------------------------------------//
Chain::~Chain () {
}

//----------------------------------------------------------------//
const Chain* Chain::choose ( const Chain& chain0, const Chain& chain1 ) {

//    size_t size0 = chain0.mCycles.size ();
//    size_t size1 = chain1.mCycles.size ();
//
//    size_t minSize = size0 < size1 ? size0 : size1;
//    for ( size_t i = 0; i < minSize; ++i ) {
//
//        const Cycle& cycle0 = *chain0.mCycles [ i ];
//        const Cycle& cycle1 = *chain1.mCycles [ i ];
//
//        int compare = Cycle::compare ( cycle0, cycle1 );
//
//        // if cycle0 is better
//        if ( compare == -1 ) {
//            return Chain::choose ( i, chain0, chain1 );
//        }
//
//        // if cycle1 is better
//        if ( compare == 1 ) {
//            return Chain::choose ( i, chain1, chain0 );
//        }
//    }
//
//    return &chain0;

    return &chain0;
}

//----------------------------------------------------------------//
const Chain* Chain::choose ( size_t cycleID, const Chain& prefer, const Chain& other ) {

//    // if other is editable, the decision is easy. go with the preferred.
//    if ( other.canEdit ( cycleID )) return &prefer;
//
//    // other is not editable, which means a critical mass of players have committed
//    // to the next cycle. so the only way we can beat it is if we have a critical
//    // mass of players later in the preferred chain. (can this happen?)
//
//    size_t max0 = prefer.findMax ( cycleID );
//    size_t max1 = other.findMax ( cycleID );
//
//    if ( max0 == max1 ) return &prefer;
//
//    if ( max1 < max0 ) {
//        float ratio = ( float )max1 / ( float )max0;
//        if ( ratio <= THRESHOLD ) return &prefer;
//    }
//
//    // other chain wins it.
//    return &other;

    return &prefer;
}

//----------------------------------------------------------------//
size_t Chain::countBlocks () {

    return this->getVersion ();
}

//----------------------------------------------------------------//
size_t Chain::countBlocks ( size_t cycleIdx ) {

    VersionedValueIterator < Cycle > cycleIt ( *this, CYCLE_KEY );
    size_t top = this->getVersion ();

    for ( ; cycleIt; cycleIt.prev ()) {
        const Cycle& cycle = *cycleIt;
        if ( cycle.mCycleID == cycleIdx ) {
            return top - cycle.mBase;
        }
        top = cycle.mBase;
    }
    return top - 0;
}

//----------------------------------------------------------------//
size_t Chain::countCycles () const {

    const Cycle& cycle = this->getTopCycle ();
    return cycle.mCycleID + 1;
}

//----------------------------------------------------------------//
size_t Chain::countParticipants ( const Cycle& cycle, string minerID ) {

    assert ( cycle.mCycleID < this->mMetaData->mCycleMetadata.size ());
    const CycleMetadata& cycleMetadata = this->mMetaData->mCycleMetadata [ cycle.mCycleID ];
    return cycleMetadata.mKnownParticipants.size () + ( minerID.size () > 0 ? ( this->isInCycle ( cycle, minerID ) ? 0 : 1 ) : 0 );
}

//----------------------------------------------------------------//
Block* Chain::findBlock ( u64 height ) {

    Block* block = NULL;

//    // TODO: replace with something more efficient
//    for ( size_t i = 0; i < this->mCycles.size (); ++i ) {
//        block = this->mCycles [ i ]->findBlock ( height );
//        if ( block ) break;
//    }
    return block;
}

//----------------------------------------------------------------//
size_t Chain::findMax ( size_t cycleID ) const {

//    size_t max = this->mCycles [ cycleID ]->countMiners ();
//
//    size_t size = this->mCycles.size ();
//    for ( size_t i = cycleID + 1; i < size; ++i ) {
//
//        size_t test = this->mCycles [ i ]->countMiners ();
//        if ( max < test ) {
//            max = test;
//        }
//    }
//    return max;

    return 0;
}

//----------------------------------------------------------------//
ChainPlacement Chain::findNextCycle ( string minerID ) {

    // first, seek back to find the earliest cycle we could change.
    // we're only allowed change if next cycle ratio is below the threshold.
    // important to measure the threshold *after* the proposed change.
    size_t nCycles = this->countCycles ();

    // the 'best' cycle is the earliest cycle in the chain that can be edited and will be 'improved.'
    // 'improved' means the miner will be added to the pool and/or chain.
    const Cycle* bestCycle = NULL; // default is to start a new cycle.

    // if only one cycle, that is the genesis cycle. skip it and add a new cycle.
    if ( nCycles > 1 ) {

        // start at the last cycle and count backward until we find a the best cycle.
        // the most recent cycle can always be edited, though may not be 'improved.'
        VersionedValueIterator < Cycle > cycleIt ( *this, CYCLE_KEY );
        
        while ( cycleIt ) {
        
            const Cycle& cycle = *cycleIt; // cycle under consideration
            
            // if we're here at all, the cycle can be edited. if it will also be improved, choose it as our 'best' cycle.
            bestCycle = this->willImprove ( cycle, minerID ) ? &cycle : bestCycle;
            
            cycleIt.prev (); // don't need to check for overrun; canEdit () will always return false for cycle 0.
            
            // if we can't edit, break.
            if ( !this->canEdit ( *cycleIt, cycle, minerID )) break;
        }
    }

    if ( bestCycle ) {
        return ChainPlacement ( *bestCycle, false );
    }
    return ChainPlacement ( this->getTopCycle (), true );
}

//----------------------------------------------------------------//
//size_t Chain::findPositionInCycle ( const Cycle& cycle, size_t score ) {
//
//    VersionedStoreIterator chainIt ( *this, cycle.mBase );
//    size_t top = this->getVersion () - cycle.mBase;
//    for ( size_t i = 0; i < top; ++i ) {
//        if ( chainIt.getValue < Block >( BLOCK_KEY ).getScore () < score ) return i;
//    }
//    return top;
//}

//----------------------------------------------------------------//
//Block& Chain::getBlock ( size_t cycleIdx, size_t blockIdx ) {
//
//    return this->getCycle ( cycleIdx ).getBlock ( blockIdx );
//}

//----------------------------------------------------------------//
//const Cycle* Chain::getCycle ( size_t idx ) {
//    
//    VersionedValueIterator < Cycle > cycleIt ( *this, CYCLE_KEY );
//    
//    for ( ; cycleIt; cycleIt.prev ()) {
//        const Cycle& cycle = *cycleIt;
//        if ( cycle.mCycleID == idx ) return &cycle;
//    }
//    return NULL;
//}

//----------------------------------------------------------------//
const Block& Chain::getTopBlock () {

    return this->getValue < Block >( BLOCK_KEY );
}

//----------------------------------------------------------------//
const Cycle& Chain::getTopCycle () const {

    return this->getValue < Cycle >( CYCLE_KEY );
}

//----------------------------------------------------------------//
bool Chain::isInCycle ( const Cycle& cycle, string minerID ) {

    VersionedStoreIterator chainIt ( *this, cycle.mBase );
    for ( ; chainIt && ( chainIt.getValue < Cycle >( CYCLE_KEY ).mCycleID == cycle.mCycleID ); chainIt.next ()) {
        if ( chainIt.getValue < Block >( BLOCK_KEY ).mMinerID == minerID ) return true;
    }
    return false;
}

//----------------------------------------------------------------//
bool Chain::isParticipant ( const Cycle& cycle, string minerID ) const {

    return this->mMetaData->isParticipant ( cycle.mCycleID, minerID );
}

//----------------------------------------------------------------//
void Chain::newCycle () {

    Cycle cycle;
    cycle.mBase = this->getVersion ();
    if ( cycle.mBase > 0 ) {
        const Cycle prevCycle = this->getValue < Cycle >( CYCLE_KEY );
        cycle.mCycleID = prevCycle.mCycleID + 1;
    }
    this->setValue < Cycle >( CYCLE_KEY, cycle );
    this->mMetaData->affirmCycle ( cycle.mCycleID );
}

//----------------------------------------------------------------//
void Chain::prepareForPush ( const ChainPlacement& placement, Block& block ) {

    assert ( placement.mCycle );

    if ( placement.mNewCycle ) {
        this->newCycle ();
    }
    else {
        const Cycle* cycle = placement.mCycle;
        
        size_t top = this->getVersion ();
        size_t truncate = cycle->mBase;
        size_t score = block.getScore ();
        VersionedStoreIterator chainIt ( *this, truncate );
        for ( ; truncate < top; ++truncate ) {
            if ( chainIt.getValue < Block >( BLOCK_KEY ).getScore () > score ) break;
        }
        this->rewind ( truncate );
    }
    
    const Block* prevBlock = &this->getValue < Block >( BLOCK_KEY );
    block.setPreviousBlock ( prevBlock );
}

//----------------------------------------------------------------//
void Chain::print ( const char* pre, const char* post ) const {

//    if ( pre ) {
//        printf ( "%s", pre );
//    }
//
//    for ( size_t i = 0; i < this->mCycles.size (); ++i ) {
//        this->mCycles [ i ]->print ();
//    }
//
//    if ( post ) {
//        printf ( "%s", post );
//    }
}

//----------------------------------------------------------------//
bool Chain::pushBlock ( Block& block ) {

    State state ( *this );

    bool result = block.apply ( state );

    if ( result ) {
        state.setValue < Block >( BLOCK_KEY, block );
        state.pushVersion ();
        this->takeSnapshot ( state );
        this->mMetaData->affirmParticipant ( block.mCycleID, block.mMinerID );
    }
    
    return result;
}

//----------------------------------------------------------------//
bool Chain::pushBlockAndSign ( Block& block, const CryptoKey& key, string hashAlgorithm ) {

    block.sign ( key, hashAlgorithm );
    return this->pushBlock ( block );
}

//----------------------------------------------------------------//
void Chain::rebuildState () {

//    this->mState.reset ();
//    for ( size_t i = 0; i < this->mCycles.size (); ++i ) {
//        if ( !this->mCycles [ i ]->apply ( this->mState )) {
//            this->mCycles.resize ( i + 1 );
//            break;
//        }
//    }
}

//----------------------------------------------------------------//
size_t Chain::size () const {

    return this->VersionedStore::getVersion ();
}

//----------------------------------------------------------------//
bool Chain::willImprove ( const Cycle& cycle, string minerID ) {

    // cycle will improve if miner is missing from either the participant set or the chain itself
    return (( cycle.mCycleID > 0 ) && (( this->isParticipant ( cycle, minerID ) && this->isInCycle ( cycle, minerID )) == false ));
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Chain::AbstractSerializable_serialize ( AbstractSerializer& serializer ) {

//    serializer.serialize ( "cycles",        this->mCycles );
//
//    if ( serializer.getMode () == AbstractSerializer::SERIALIZE_IN ) {
//        this->rebuildState ();
//    }
}

} // namespace Volition
