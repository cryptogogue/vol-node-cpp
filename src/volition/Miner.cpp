// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Miner.h>
#include <volition/TheContext.h>
#include <volition/SyncChainTask.h>

namespace Volition {

//================================================================//
// Miner
//================================================================//

//----------------------------------------------------------------//
Chain* Miner::getChain () const {

    return this->mChain ? this->mChain.get () : 0;
}

//----------------------------------------------------------------//
string Miner::getMinerID () const {

    return this->mMinerID;
}

//----------------------------------------------------------------//
const State& Miner::getState () const {

    assert ( this->mChain );
    return this->mChain->getState ();
}

//----------------------------------------------------------------//
void Miner::loadGenesis ( string path ) {

    fstream inStream;
    inStream.open ( path, ios_base::in );

    unique_ptr < Block > block = make_unique < Block >();
    FromJSONSerializer::fromJSON ( *block, inStream );

    this->setGenesis ( move ( block ));
}

//----------------------------------------------------------------//
void Miner::loadKey ( string keyfile, string password ) {

    // TODO: password

    fstream inStream;
    inStream.open ( keyfile, ios_base::in );
    Volition::FromJSONSerializer::fromJSON ( this->mKeyPair, inStream );
}

//----------------------------------------------------------------//
unique_ptr < Block > Miner::makeBlock ( const Chain& chain ) {
    unique_ptr < Block > block = make_unique < Block >();

    State state = chain.getState ();

    list < shared_ptr < AbstractTransaction >>::iterator transactionIt = this->mPendingTransactions.begin ();
    for ( ; transactionIt != this->mPendingTransactions.end (); ++transactionIt ) {
        shared_ptr < AbstractTransaction > transaction = *transactionIt;
        if ( transaction->apply ( state )) {
            block->pushTransaction ( transaction );
        }
    }
    return block;
}

//----------------------------------------------------------------//
void Miner::pushBlock ( Chain& chain, bool force ) {

    ChainPlacement placement = chain.findPlacement ( this->mMinerID, force );
    if ( placement.canPush ()) {
        unique_ptr < Block > block = this->makeBlock ( chain );
        
        if ( !( this->mLazy && ( block->countTransactions () == 0 ))) {
        
            block->setMinerID ( this->mMinerID );
            bool result = chain.pushAndSign ( placement, move ( block ), this->mKeyPair );
            assert ( result );
        }
    }
}

//----------------------------------------------------------------//
void Miner::pushTransaction ( shared_ptr < AbstractTransaction > transaction ) {

    this->mPendingTransactions.push_back ( transaction );
}

//----------------------------------------------------------------//
void Miner::setLazy ( bool lazy ) {

    this->mLazy = lazy;
}

//----------------------------------------------------------------//
void Miner::setMinerID ( string minerID ) {

    this->mMinerID = minerID;
}

//----------------------------------------------------------------//
Miner::Miner () :
    mLazy ( false ) {
}

//----------------------------------------------------------------//
Miner::~Miner () {
}

//----------------------------------------------------------------//
void Miner::setGenesis ( shared_ptr < Block > block ) {
    
    unique_ptr < Chain > chain = make_unique < Chain >( block );
    
    if ( chain->countCycles () > 0 ) {
        this->mChain = move ( chain );
    }
}

//----------------------------------------------------------------//
void Miner::updateChain ( unique_ptr < Chain > proposedChain ) {

    if ( !proposedChain ) return;
    
    this->pushBlock ( *proposedChain, false );
    
    if ( this->mChain.get () != Chain::choose ( *this->mChain, *proposedChain )) {
        this->mChain = move ( proposedChain );
    }
}

} // namespace Volition
