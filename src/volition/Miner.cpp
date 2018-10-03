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
void Miner::addTransactions ( Chain& chain, Block& block ) {

    State state;
    state.takeSnapshot ( chain );

    list < shared_ptr < AbstractTransaction >>::iterator transactionIt = this->mPendingTransactions.begin ();
    for ( ; transactionIt != this->mPendingTransactions.end (); ++transactionIt ) {
        shared_ptr < AbstractTransaction > transaction = *transactionIt;
        if ( transaction->apply ( state )) {
            block.pushTransaction ( transaction );
        }
    }
}

//----------------------------------------------------------------//
Digest Miner::computeAllure ( size_t cycleID ) const {

    Poco::Crypto::ECDSADigestEngine signature ( this->mKeyPair, Signature::DEFAULT_HASH_ALGORITHM );
    Poco::DigestOutputStream signatureStream ( signature );
    signatureStream << cycleID;
    signatureStream.close ();
    
    return signature.signature ();
}

//----------------------------------------------------------------//
size_t Miner::computeScore ( const Digest& allure ) const {

    if ( TheContext::get ().getScoringMode () == TheContext::ScoringMode::ALLURE ) {
        string allureString = Poco::DigestEngine::digestToHex ( allure );
        return std::hash < string >{}( allureString );
    }
    return strtol ( this->mMinerID.c_str (), 0, 10 );
}

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
    return *this->mChain;
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
void Miner::pushBlock ( Chain& chain, bool force ) {

    if ( chain.canPush ( this->mMinerID, force )) {

        // find the cycle (including a new cycle) that the block should be placed in.
        ChainPlacement placement = chain.findNextCycle ( this->mMetadata, this->mMinerID );
        
        // this also computes the allure for that cycle.
        Block block ( this->mMinerID, placement.getCycleID (), this->mKeyPair, Signature::DEFAULT_HASH_ALGORITHM );
        
        Chain fork ( chain );
        fork.prepareForPush ( this->mMetadata, placement, block );
        
        // do this *after* prepare
        this->addTransactions ( fork, block );
        
        if ( !( this->mLazy && ( block.countTransactions () == 0 ))) {
            
            block.sign ( this->mKeyPair, Signature::DEFAULT_HASH_ALGORITHM );
            bool result = fork.pushBlock ( block );
            assert ( result );
            
            this->mMetadata.affirmParticipant ( block.getCycleID (), block.getMinerID ());
            chain.takeSnapshot ( fork );
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
    
    unique_ptr < Chain > chain = make_unique < Chain >();
    chain->pushBlock ( *block );
    
    if ( chain->countCycles () > 0 ) {
        this->mChain = move ( chain );
    }
}

//----------------------------------------------------------------//
void Miner::updateChain ( Chain& proposedChain ) {
    
    if ( this->mChain ) {
        this->mChain->update ( this->mMetadata, proposedChain );
        this->pushBlock ( *this->mChain, false );
    }
}

} // namespace Volition
