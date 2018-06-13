// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <Chain.h>
#include <simulator/context.h>

namespace Volition {

//================================================================//
// Chain
//================================================================//

//----------------------------------------------------------------//
void Chain::apply ( State& state ) const {

    for ( size_t i = 0; i < this->mCycles.size (); ++i ) {
        this->mCycles [ i ]->apply ( state );
    }
}

//----------------------------------------------------------------//
bool Chain::canEdit ( size_t cycleID, string minerID ) const {

    assert ( cycleID < this->mCycles.size ());
    if ( cycleID == this->mCycles.back ()->getID ()) return true;
    
    const Cycle& cycle0     = *this->mCycles [ cycleID ];
    const Cycle& cycle1     = *this->mCycles [ cycleID + 1 ];

    size_t size0            = cycle0.countMiners ( minerID );
    size_t size1            = cycle1.countMiners ();

    float ratio             = ( float )size1 / ( float )size0;

    return ratio <= Context::GetThreshold ();
}

//----------------------------------------------------------------//
bool Chain::canEdit ( size_t cycleID, const Chain& chain ) const {

    if ( this->canEdit ( cycleID )) return true;
    
    size_t participants0 = this->mCycles [ cycleID ]->countMiners ();
    size_t participants1 = chain.mCycles [ cycleID ]->countMiners ();
    
    if ( participants0 < participants1 ) {
    
        float ratio = ( float )participants0 / ( float )participants1;
        if ( ratio <= Context::GetThreshold ()) return true;
    }
    
    size_t size0 = this->mCycles.size ();
    size_t size1 = chain.mCycles.size ();
    
    size_t min = ( size0 < size1 ) ? size0 : size1;
    
    for ( size_t i = cycleID + 1; i < ( min - 1 ); ++i ) {
        if ( this->mCycles [ i ]->countMiners () < chain.mCycles [ i ]->countMiners ()) {
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------//
Chain::Chain () {
}

//----------------------------------------------------------------//
Chain::~Chain () {
}

//----------------------------------------------------------------//
const Chain* Chain::choose ( const Chain& chain0, const Chain& chain1 ) {

    size_t size0 = chain0.mCycles.size ();
    size_t size1 = chain1.mCycles.size ();

    size_t minSize = size0 < size1 ? size0 : size1;
    for ( size_t i = 0; i < minSize; ++i ) {
    
        const Cycle& cycle0 = *chain0.mCycles [ i ];
        const Cycle& cycle1 = *chain1.mCycles [ i ];

        int compare = Cycle::compare ( cycle0, cycle1 );

        // if cycle0 is better
        if ( compare == -1 ) {
            return Chain::choose ( i, chain0, chain1 );
        }
        
        // if cycle1 is better
        if ( compare == 1 ) {
            return Chain::choose ( i, chain1, chain0 );
        }
    }
    
    return &chain0;
}

//----------------------------------------------------------------//
const Chain* Chain::choose ( size_t cycleID, const Chain& prefer, const Chain& other ) {

    // if other is editable, the decision is easy. go with the preferred.
    if ( other.canEdit ( cycleID )) return &prefer;
    
    // other is not editable, which means a critical mass of players have committed
    // to the next cycle. so the only way we can beat it is if we have a critical
    // mass of players later in the preferred chain. (can this happen?)

    size_t max0 = prefer.findMax ( cycleID );
    size_t max1 = other.findMax ( cycleID );

    if ( max0 == max1 ) return &prefer;

    if ( max1 < max0 ) {
        float ratio = ( float )max1 / ( float )max0;
        if ( ratio <= Context::GetThreshold ()) return &prefer;
    }

    // other chain wins it.
    return &other;
}

//----------------------------------------------------------------//
size_t Chain::findMax ( size_t cycleID ) const {

    size_t max = this->mCycles [ cycleID ]->countMiners ();

    size_t size = this->mCycles.size ();
    for ( size_t i = cycleID + 1; i < size; ++i ) {
    
        size_t test = this->mCycles [ i ]->countMiners ();
        if ( max < test ) {
            max = test;
        }
    }
    return max;
}

//----------------------------------------------------------------//
Cycle* Chain::getTopCycle () {

    return this->mCycles.size () > 0 ? this->mCycles.back ().get () : 0;
}

//----------------------------------------------------------------//
const Cycle* Chain::getTopCycle () const {

    return this->mCycles.size () > 0 ? this->mCycles.back ().get () : 0;
}

//----------------------------------------------------------------//
Cycle* Chain::nextCycle ( string minerID, bool force ) {

    Cycle* topCycle = this->getTopCycle ();
    if (( topCycle && topCycle->isInChain ( minerID )) && !force ) return 0;

    // first, seek back to find the earliest cycle we could change.
    // we're only allowed change if next cycle ratio is below the threshold.
    // important to measure the threshold *after* the proposed change.
    size_t nCycles = this->mCycles.size ();
    if ( nCycles > 0 ) {
    
        // start at the last cycle and count backward until we find a cycle we
        // aren't allowed to change. we want the cycle after that (which may be
        // the last cycle in the chain).
        size_t baseCycleID = nCycles - 1;
        for ( ; baseCycleID > 0; --baseCycleID ) {
            if ( !this->canEdit ( baseCycleID - 1, minerID )) break;
        }
        
        // now count forward from the base until we find a cycle that we'd want to change.
        // again, base may be the last cycle in the chain.
        // we may not find one, in which case we need to add a new cycle.
        for ( size_t i = baseCycleID; i < nCycles; ++i ) {
            Cycle* cycle = this->mCycles [ i ].get ();
            if ( cycle->willImprove ( minerID )) {
                this->mCycles.resize ( i + 1 );
                return cycle;
            }
        }
    }

    // add a new cycle.
    int cycleID = ( int )this->mCycles.size ();
    this->mCycles.push_back ( make_unique < Cycle >());
    Cycle* cycle = this->getTopCycle ();
    cycle->setID ( cycleID );
    return cycle;
}

//----------------------------------------------------------------//
void Chain::print ( const char* pre, const char* post ) const {

    if ( pre ) {
        printf ( "%s", pre );
    }

    for ( size_t i = 0; i < this->mCycles.size (); ++i ) {
        this->mCycles [ i ]->print ();
    }
    
    if ( post ) {
        printf ( "%s", post );
    }
}

//----------------------------------------------------------------//
bool Chain::verify ( const State& state ) const {

    // TODO: verify chain structure (order, participants, etc.)

    for ( size_t i = 0; i < this->mCycles.size (); ++i ) {
        if ( !this->mCycles [ i ]->verify ( state )) return false;
    }
    return true;
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Chain::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {
    
    const Poco::JSON::Array::Ptr cycles = object.getArray ( "cycles" );
    this->mCycles.resize ( cycles->size ());

    for ( size_t i = 0; i < cycles->size (); ++i ) {

        const Poco::JSON::Object::Ptr cycleJSON = cycles->getObject (( unsigned int )i );
        assert ( cycleJSON );

        this->mCycles [ i ] = make_unique < Cycle >();
        this->mCycles [ i ]->fromJSON ( *cycleJSON );
    }
}

//----------------------------------------------------------------//
void Chain::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
    
    Poco::JSON::Array::Ptr cycles = new Poco::JSON::Array ();
    object.set ( "cycles", cycles );
    
    for ( size_t i = 0; i < this->mCycles.size (); ++i ) {
        cycles->set (( unsigned int )i, this->mCycles [ i ]->toJSON ());
    }
}

} // namespace Volition
