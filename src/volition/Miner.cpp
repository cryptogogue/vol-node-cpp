// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractHashable.h>
#include <volition/Block.h>
#include <volition/Genesis.h>
#include <volition/Miner.h>
#include <volition/SyncChainTask.h>

namespace Volition {

//================================================================//
// Miner
//================================================================//

//----------------------------------------------------------------//
const Chain* Miner::getChain () const {

    return this->mChain ? this->mChain.get () : 0;
}

//----------------------------------------------------------------//
//string Miner::getPublicKey () {
//
//    stringstream strStream;
//    this->mKeyPair->save ( &strStream );
//    return strStream.str ();
//}

//----------------------------------------------------------------//
void Miner::loadGenesis ( string path ) {
    
    fstream inStream;
    inStream.open ( path, ios_base::in );
    
    unique_ptr < Block > block = make_unique < Block >();
    block->fromJSON ( inStream );
    
    this->setGenesis ( move ( block ));
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
        block->setMinerID ( this->mMinerID );
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
void Miner::setGenesis ( shared_ptr < Block > block ) {

    this->mState = State ();
    
    unique_ptr < Chain > chain = make_unique < Chain >( block );
    
    if ( chain->verify ()) {
    
        this->mChain = move ( chain );
        this->mChain->apply ( this->mState );
    }
}

//----------------------------------------------------------------//
void Miner::updateChain ( unique_ptr < Chain > proposedChain ) {

    if ( !proposedChain ) return;
    //if ( !proposedChain->verify ( this->mState )) return; // TODO: move to task thread; use snapshot of state when task created
    
    this->pushBlock ( *proposedChain, false );
    
    if ( this->mChain.get () != Chain::choose ( *this->mChain, *proposedChain )) {
        this->mChain = move ( proposedChain );
    }
}

} // namespace Volition
