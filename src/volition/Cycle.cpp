// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Cycle.h>
#include <volition/TheContext.h>

namespace Volition {

//================================================================//
// Cycle
//================================================================//

//----------------------------------------------------------------//
bool Cycle::apply ( State& state ) {

    for ( size_t i = 0; i < this->mBlocks.size (); ++i ) {
        if ( !this->mBlocks [ i ]->apply ( state )) {
            this->mBlocks.resize ( i + 1 );
            this->rebuildMinerSet ();
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------//
int Cycle::compare ( const Cycle& cycle0, const Cycle& cycle1 ) {

    size_t size0 = cycle0.mBlocks.size ();
    size_t size1 = cycle1.mBlocks.size ();

    size_t minSize = size0 < size1 ? size0 : size1;
    for ( size_t i = 0; i < minSize; ++i ) {

        size_t score0 = cycle0.mBlocks [ i ]->getScore ();
        size_t score1 = cycle1.mBlocks [ i ]->getScore ();

        if ( score0 != score1 ) {
            return score0 < score1 ? -1 : 1;
        }
    }

    if ( size0 != size1 ) {
        return size0 > size1 ? -1 : 1;
    }

    size_t players0 = cycle0.mMiners.size ();
    size_t players1 = cycle1.mMiners.size ();

    if ( players0 != players1 ) {
        return players0 > players1 ? -1 : 1;
    }
    
    return 0;
}

//----------------------------------------------------------------//
bool Cycle::containsMiner ( string minerID ) const {
    
    return this->mMiners.find ( minerID ) != this->mMiners.end ();
}

//----------------------------------------------------------------//
size_t Cycle::countBlocks () const {

    return this->mBlocks.size ();
}

//----------------------------------------------------------------//
size_t Cycle::countMiners ( string minerID ) const {

    return this->mMiners.size () + ( minerID.size () > 0 ? ( this->containsMiner ( minerID ) ? 0 : 1 ) : 0 );
}

//----------------------------------------------------------------//
Cycle::Cycle () :
    mCycleID ( 0 ) {
}

//----------------------------------------------------------------//
Cycle::~Cycle () {
}

//----------------------------------------------------------------//
Block* Cycle::findBlock ( u64 height ) {

    if (( this->mBlocks.size () > 0 ) && ( this->mBlocks.front ()->mHeight <= height ) && ( this->mBlocks.back ()->mHeight >= height )) {

        // TODO: replace with something more efficient
        for ( size_t i = 0; i < this->mBlocks.size (); ++i ) {
            Block* block = this->mBlocks [ i ].get ();
            if ( block->mHeight == height ) return block;
        }
    }
    return NULL;
}

//----------------------------------------------------------------//
u64 Cycle::findPosition ( size_t score ) const {

    u64 position = 0;
    for ( u64 i = 0; i < this->mBlocks.size (); ++i, ++position ) {
       size_t test = this->mBlocks [ i ]->getScore ();
        if ( score < test ) break;
    }
    return position;
}

//----------------------------------------------------------------//
Block& Cycle::getBlock ( size_t idx ) {

    assert (( idx < this->mBlocks.size () ) && ( this->mBlocks [ idx ]));
    return *this->mBlocks [ idx ];
}

//----------------------------------------------------------------//
size_t Cycle::getID () const {

    return this->mCycleID;
}

//----------------------------------------------------------------//
size_t Cycle::getLength () const {

    return this->mBlocks.size ();
}

//----------------------------------------------------------------//
bool Cycle::isInChain ( string minerID ) const {

    for ( size_t i = 0; i < this->mBlocks.size (); ++i ) {
        if ( this->mBlocks [ i ]->getMinerID () == minerID ) return true;
    }
    return false;
}

//----------------------------------------------------------------//
void Cycle::print () const {

    if ( this->mCycleID == 0 ) {
        if ( this->mBlocks.size () > 0 ) {
            printf ( "[.]" );
        }
        else {
            printf ( "[]" );
        }
        return;
    }

    printf ( "[" );
    for ( size_t i = 0; i < this->mBlocks.size (); ++i ) {
        const Block& block = *this->mBlocks [ i ];
        if ( i > 0 ) {
            printf ( "," );
        }
        printf ( "%s", block.getMinerID ().c_str ());
    }
    //printf ( ",(%d,%g)]", ( int )this->mPlayers.size (), this->mNewPlayerRatio );
    printf ( " (%d)]", ( int )this->mMiners.size ());
}

//----------------------------------------------------------------//
void Cycle::rebuildMinerSet () {

    this->mMiners.clear ();

    for ( size_t i = 0; i < this->mBlocks.size (); ++i ) {
        string minerID = this->mBlocks [ i ]->getMinerID ();
        assert ( this->mMiners.find ( minerID ) == this->mMiners.end ());
        this->mMiners.insert ( minerID );
    }
}

//----------------------------------------------------------------//
void Cycle::setID ( u64 cycleID ) {

    this->mCycleID = cycleID;
}

//----------------------------------------------------------------//
bool Cycle::willImprove ( string minerID ) const {

    // cycle will improve if miner is missing from either the participant set or the chain itself
    return (( this->mCycleID > 0 ) && (( this->containsMiner ( minerID ) && this->isInChain ( minerID )) == false ));
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Cycle::AbstractSerializable_serialize ( AbstractSerializer& serializer ) {

    serializer.serialize ( "cycleID",       this->mCycleID );
    serializer.serialize ( "blocks",        this->mBlocks );
    
    if ( serializer.getMode () == AbstractSerializer::SERIALIZE_IN ) {
        this->rebuildMinerSet ();
    }
}

} // namespace Volition

