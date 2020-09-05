// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractChainRecorder.h>
#include <volition/Block.h>
#include <volition/Digest.h>
#include <volition/Miner.h>
#include <volition/Miner.json.h>
#include <volition/TheContext.h>
#include <volition/SyncChainTask.h>

namespace Volition {

//================================================================//
// Miner
//================================================================//

//----------------------------------------------------------------//
void Miner::affirmKey () {

    if ( !this->mKeyPair ) {
        this->mKeyPair.elliptic ();
    }
    assert ( this->mKeyPair );
}

//----------------------------------------------------------------//
bool Miner::checkBestBranch ( string miners ) const {

    assert ( this->mChain );
    return this->mChain->checkMiners ( miners );
}

//----------------------------------------------------------------//
void Miner::checkEnvironment () {

    assert ( sizeof ( s8 ) == 1 );
    assert ( sizeof ( s16 ) == 2 );
    assert ( sizeof ( s32 ) == 4 );
    assert ( sizeof ( s64 ) == 8 );
    
    assert ( sizeof ( u8 ) == 1 );
    assert ( sizeof ( u16 ) == 2 );
    assert ( sizeof ( u32 ) == 4 );
    assert ( sizeof ( u64 ) == 8 );
    
    assert ( sizeof ( time_t ) == 8 );
    assert ( sizeof ( size_t ) == 8 );
            
    SerializableSet < string > hashSet;
    
    hashSet.insert ( "abc" );
    assert ( ToJSONSerializer::toDigestString ( hashSet ) == JSON_STR (["abc"]));
    assert ( Digest ( hashSet ).toHex () == "02f393ea9358560882c1fe797bf99d600aa4643a68276d8e3d714d1c4f19aecc" );
    
    hashSet.clear ();
    hashSet.insert ( "c" );
    hashSet.insert ( "b" );
    hashSet.insert ( "a" );
    assert ( ToJSONSerializer::toDigestString ( hashSet ) == JSON_STR (["a","b","c"]));
    assert ( Digest ( hashSet ).toHex () == "fa1844c2988ad15ab7b49e0ece09684500fad94df916859fb9a43ff85f5bb477" );

    SerializableMap < string, string > hashMap;

    hashMap [ "x" ] = "abc";
    assert ( ToJSONSerializer::toDigestString ( hashMap ) == JSON_STR ({"x":"abc"}));
    assert ( Digest ( hashMap ).toHex () == "270012bdffcdc54c226ac7d3bcc965b48e8c688c39b78313e40c82d96cda2dd4" );

    hashMap.clear ();
    hashMap [ "z" ] = "c";
    hashMap [ "y" ] = "b";
    hashMap [ "x" ] = "a";
    assert ( ToJSONSerializer::toDigestString ( hashMap ) == JSON_STR ({"x":"a","y":"b","z":"c"}));
    assert ( Digest ( hashMap ).toHex () == "a37e1584a03d065fcd5320ded38b0e0091e037cbadcdaf10df8f55e0d60c823d" );

    SerializableMap < string, u64 > hashMap64;
    hashMap64 [ "u32" ] = 0xffffffff;
    hashMap64 [ "u64" ] = ( u64 )0xffffffff + 1;
    hashMap64 [ "max" ] = ( u64 )0xffffffffffffffff;

    string hashMap64JSON = ToJSONSerializer::toDigestString ( hashMap64 );
    assert ( hashMap64JSON == JSON_STR ({"max":"0xffffffffffffffff","u32":4294967295,"u64":"0x100000000"}));
    assert ( Digest ( hashMap64 ).toHex () == "66c456477effe884fcbededc2422e37dd6d77215f0a3f3702104ab1b0cee484f" );

    hashMap64.clear ();
    FromJSONSerializer::fromJSONString ( hashMap64, hashMap64JSON );
    assert ( hashMap64 [ "u32" ] == 0xffffffff );
    assert ( hashMap64 [ "u64" ] == ( u64 )0xffffffff + 1 );
    assert ( hashMap64 [ "max" ] == ( u64 )0xffffffffffffffff );

    Block hashBlock;
    FromJSONSerializer::fromJSONString ( hashBlock, test_block_json );
    assert ( ToJSONSerializer::toDigestString ( hashBlock ) == JSON_STR ({"height":0,"time":0,"transactions":[]}));
    assert ( Digest ( hashBlock ).toHex () == "8f7383032c626071fde10fad55814c2107e4ed87f36e8370b36d10ad1f2870bc" );
}

//----------------------------------------------------------------//
bool Miner::controlPermitted () const {

    return this->mControlPermitted;
}

//----------------------------------------------------------------//
void Miner::extend ( bool force ) {
    
    assert ( this->mChain );
    Chain& chain = *this->mChain;
    
    time_t now = this->getTime ();
    
    if ( !force ) {
        shared_ptr < Block > topBlock = chain.getBlock ();
        assert ( topBlock );
        if (( this->mMinerID == topBlock->getMinerID ()) && topBlock->isInRewriteWindow ( now )) return;
    }

    size_t nBlocks = chain.countBlocks ();
    size_t top = nBlocks;
    
    for ( size_t i = nBlocks; i > 1; --i ) {
        
        // we can replace any block more recent than the current time minus the time window.
        shared_ptr < Block > rivalBlock = chain.getBlock ( i - 1 );
        if ( !rivalBlock->isInRewriteWindow ( now )) break;
        if ( rivalBlock->getMinerID () == this->mMinerID ) break; // don't overwrite own block.
        
        // prepare test block
        shared_ptr < Block > prevBlock = chain.getBlock ( i - 2 );
        Block testBlock ( this->mMinerID, now, prevBlock.get (), this->mKeyPair, Digest::DEFAULT_HASH_ALGORITHM );
        
        if ( Block::compare ( testBlock, *rivalBlock ) <= 0 ) {
            top = i - 1;
        }
    }
    
    Chain fork ( chain );
    if ( top < nBlocks ) {
        fork.revert ( top - 1 );
        fork.pushVersion ();
    }
    
    shared_ptr < Block > prevBlock = fork.getBlock ();
    assert ( prevBlock );
    
    Block block ( this->mMinerID, now, prevBlock.get (), this->mKeyPair, Digest::DEFAULT_HASH_ALGORITHM );
    this->fillBlock ( fork, block );
    
    if ( !( this->mLazy && ( block.countTransactions () == 0 ))) {
    
        block.sign ( this->mKeyPair, Digest::DEFAULT_HASH_ALGORITHM );
        bool result = fork.pushBlock ( block, this->mBlockVerificationPolicy );
        assert ( result );
        
        chain.takeSnapshot ( fork );
    }
}

//----------------------------------------------------------------//
const Chain* Miner::getBestBranch () const {

    return this->mChain.get ();
}

//----------------------------------------------------------------//
const BlockTree& Miner::getBlockTree () const {

    return this->mBlockTree;
}

//----------------------------------------------------------------//
const CryptoKey& Miner::getKeyPair () const {

    return this->mKeyPair;
}

//----------------------------------------------------------------//
bool Miner::getLazy () const {

    return this->mLazy;
}

//----------------------------------------------------------------//
Ledger& Miner::getLedger () {

    assert ( this->mChain );
    return *this->mChain;
}

//----------------------------------------------------------------//
const Ledger& Miner::getLedger () const {

    assert ( this->mChain );
    return *this->mChain;
}

//----------------------------------------------------------------//
string Miner::getMinerID () const {

    return this->mMinerID;
}

//----------------------------------------------------------------//
time_t Miner::getTime () const {

    return this->Miner_getTime ();
}

//----------------------------------------------------------------//
void Miner::loadGenesis ( string path ) {

    fstream inStream;
    inStream.open ( path, ios_base::in );
    assert ( inStream.is_open ());

    shared_ptr < Block > block = make_shared < Block >();
    FromJSONSerializer::fromJSON ( *block, inStream );
    this->setGenesis ( block );
}

//----------------------------------------------------------------//
void Miner::loadKey ( string keyfile, string password ) {
    UNUSED ( password );

    // TODO: password

    fstream inStream;
    inStream.open ( keyfile, ios_base::in );
    assert ( inStream.is_open ());
    
    Volition::FromJSONSerializer::fromJSON ( this->mKeyPair, inStream );
    assert ( this->mKeyPair );
}

//----------------------------------------------------------------//
Miner::Miner () :
    mLazy ( false ),
    mControlPermitted ( false ),
    mBlockVerificationPolicy ( Block::VerificationPolicy::NONE ) {
    
    Miner::checkEnvironment ();
}

//----------------------------------------------------------------//
Miner::~Miner () {
}

//----------------------------------------------------------------//
void Miner::permitControl ( bool permit ) {

    this->mControlPermitted = permit;
}

//----------------------------------------------------------------//
shared_ptr < Block > Miner::prepareBlock () {
        
    shared_ptr < Block > prevBlock = this->mChain->getBlock ();
    assert ( prevBlock );
    
    shared_ptr < Block > block = make_shared < Block >( this->mMinerID, this->getTime (), prevBlock.get (), this->mKeyPair, Digest::DEFAULT_HASH_ALGORITHM );
    this->fillBlock ( *this->mChain, *block );
    
    if ( !( this->mLazy && ( block->countTransactions () == 0 ))) {
        block->sign ( this->mKeyPair, Digest::DEFAULT_HASH_ALGORITHM );
        return block;
    }
    return NULL;
}

//----------------------------------------------------------------//
void Miner::pushBlock ( shared_ptr < const Block > block ) {

    bool result = this->mChain->pushBlock ( *block, this->mBlockVerificationPolicy );
    assert ( result );
    
    this->mTag.mark ( this->mBlockTree.affirmBlock ( block ));
}

//----------------------------------------------------------------//
void Miner::setChainRecorder ( shared_ptr < AbstractChainRecorder > chainRecorder ) {

    this->mChainRecorder = chainRecorder;
    if ( this->mChainRecorder ) {
        this->mChainRecorder->loadChain ( *this );
    }
}

//----------------------------------------------------------------//
void Miner::setGenesis ( shared_ptr < const Block > block ) {
    
    assert ( block );
    
    shared_ptr < Chain > chain = make_shared < Chain >();
    this->mChain = chain;
    
    this->pushBlock ( block );
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
void Miner::rebuildChain () {

    this->mChain->reset ( 1 );
    this->rebuildChainRecurse ( this->mTag );
}

//----------------------------------------------------------------//
void Miner::rebuildChainRecurse ( shared_ptr < const BlockTreeNode > node ) {

    if ( node == NULL ) return;
    if ( node->getBlock ().isGenesis ()) return;
    
    this->rebuildChainRecurse ( node->getParent ());
    this->mChain->pushBlock ( node->getBlock (), this->mBlockVerificationPolicy );
}

//----------------------------------------------------------------//
void Miner::reset () {

    this->TransactionQueue::reset ();
    this->mChain->reset ( 1 );
    this->mChain->clearSchemaCache ();
    if ( this->mChainRecorder ) {
        this->mChainRecorder->reset ();
    }
    this->Miner_reset ();
}

//----------------------------------------------------------------//
void Miner::saveChain () {

    if ( this->mChainRecorder ) {
        this->mChainRecorder->saveChain ( *this );
    }
}

//----------------------------------------------------------------//
void Miner::shutdown ( bool kill ) {

    this->Miner_shutdown ( kill );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Miner::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    UNUSED ( serializer );
    
//    serializer.serialize ( "chain", this->mChain );
}

//----------------------------------------------------------------//
void Miner::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
    UNUSED ( serializer );

//    serializer.serialize ( "chain", this->mChain );
}

//----------------------------------------------------------------//
time_t Miner::Miner_getTime () const {

    time_t now;
    time ( &now );
    return now;
}

//----------------------------------------------------------------//
void Miner::Miner_reset () {
}

} // namespace Volition
