// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "AbstractHashable.h"
#include "Block.h"
#include "Genesis.h"
#include "Miner.h"
#include "SyncChainTask.h"

namespace Volition {

//================================================================//
// Miner
//================================================================//

//----------------------------------------------------------------//
const Chain* Miner::getChain () const {

    return this->mChain ? this->mChain.get () : 0;
}

//----------------------------------------------------------------//
string Miner::getPublicKey () {

    stringstream strStream;
    this->mKeyPair->save ( &strStream );
    return strStream.str ();
}

//----------------------------------------------------------------//
void Miner::loadGenesis ( string genesis ) {
    
    fstream inStream;
    inStream.open ( genesis, ios_base::in );
    
    unique_ptr < Block > block = make_unique < Block >();
    block->fromJSON ( inStream );

    this->mState = State ();
    
    unique_ptr < Chain > chain = make_unique < Chain >( move ( block ));
    
    if ( chain->verify ( this->mState )) {
    
        this->mChain = move ( chain );
        this->mChain->apply ( this->mState );
    }
}

//----------------------------------------------------------------//
void Miner::loadKey ( string keyfile, string password ) {

    this->mKeyPair = make_unique < Poco::Crypto::ECKey >( "", keyfile, password );
}

//----------------------------------------------------------------//
void Miner::pushBlock ( Chain& chain, bool force ) {

    ChainPlacement placement = chain.findPlacement ( this->mMinerID, force );
    if ( placement.canPush ()) {
        unique_ptr < Block > block = make_unique < Block >();
        chain.pushAndSign ( placement, move ( block ), *this->mKeyPair );
    }
}

//----------------------------------------------------------------//
void Miner::pushTransaction ( unique_ptr < AbstractTransaction >& transaction ) {

    this->mPendingTransactions.push_back ( move ( transaction ));
}

//----------------------------------------------------------------//
void Miner::setMinerID ( string minerID ) {

    this->mMinerID = minerID;
}

//----------------------------------------------------------------//
Miner::Miner () {
}

//----------------------------------------------------------------//
Miner::~Miner () {
}

//----------------------------------------------------------------//
void Miner::updateChain ( unique_ptr < Chain > proposedChain ) {

    if ( !proposedChain ) return;
    
    proposedChain->verify ( this->mState ); // TODO: move to task thread; use snapshot of state when task created
    
    this->pushBlock ( *proposedChain, false );
    
    if ( this->mChain.get () != Chain::choose ( *this->mChain, *proposedChain )) {
        this->mChain = move ( proposedChain );
    }
    this->mChain->print ();
}

} // namespace Volition
