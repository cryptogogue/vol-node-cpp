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

    Ledger ledger;
    ledger.takeSnapshot ( chain );

    list < shared_ptr < AbstractTransaction >>::iterator transactionIt = this->mPendingTransactions.begin ();
    for ( ; transactionIt != this->mPendingTransactions.end (); ++transactionIt ) {
        shared_ptr < AbstractTransaction > transaction = *transactionIt;
        
        // TODO: don't need to fully apply; should just check nonce and then sig
        if ( transaction->verify ( ledger )) {
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
void Miner::extendChain () {

    this->pushBlock ( this->mChain, true );
}

//----------------------------------------------------------------//
const Chain& Miner::getChain () const {

    return this->mChain;
}

//----------------------------------------------------------------//
size_t Miner::getChainSize () const {

    return this->mChain.size ();
}

//----------------------------------------------------------------//
bool Miner::getLazy () const {

    return this->mLazy;
}

//----------------------------------------------------------------//
const Ledger& Miner::getLedger () const {

    return this->mChain;
}

//----------------------------------------------------------------//
string Miner::getMinerID () const {

    return this->mMinerID;
}

//----------------------------------------------------------------//
void Miner::loadGenesis ( string path ) {

    fstream inStream;
    inStream.open ( path, ios_base::in );

    Block block;
    FromJSONSerializer::fromJSON ( block, inStream );

    this->setGenesis ( block );
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
void Miner::saveChain () {

    if ( this->mPersistenceProvider ) {
        this->mChain.persist ( this->mPersistenceProvider, MASTER_BRANCH );
        this->mPersistenceProvider->flush ();
    }
}

//----------------------------------------------------------------//
void Miner::setPersistenceProvider ( shared_ptr < AbstractPersistenceProvider > persistence ) {

    this->mPersistenceProvider = persistence;
    if ( persistence ) {
        this->mChain.takeSnapshot ( persistence, MASTER_BRANCH );
    }
    else {
        this->mChain.clear ();
    }
}

//----------------------------------------------------------------//
void Miner::setGenesis ( const Block& block ) {
    
    this->mChain.reset ();
    this->mChain.pushBlock ( block );
}

//----------------------------------------------------------------//
void Miner::updateChain ( const Chain& proposedChain ) {
    
    this->mChain.update ( this->mMetadata, proposedChain );
    this->pushBlock ( this->mChain, false );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Miner::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    
    serializer.serialize ( "chain", this->mChain );
    serializer.serialize ( "metadata", this->mMetadata );
}

//----------------------------------------------------------------//
void Miner::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "chain", this->mChain );
    serializer.serialize ( "metadata", this->mMetadata );
}

} // namespace Volition
