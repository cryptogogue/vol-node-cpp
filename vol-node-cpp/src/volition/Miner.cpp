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
bool Miner::checkBestBranch ( string miners ) const {

    assert ( this->mBestBranch );
    return this->mBestBranch->checkMiners ( miners );
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
bool Miner::controlPerimitted () const {

    return this->mControlPermitted;
}

//----------------------------------------------------------------//
size_t Miner::countBranches () const {

    return this->mBranches.size ();
}

//----------------------------------------------------------------//
void Miner::extend ( bool force ) {
    
    assert ( this->mBestBranch );
    Chain& chain = *this->mBestBranch;
    
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
        bool result = fork.pushBlock ( block );
        assert ( result );
        
        chain.takeSnapshot ( fork );
    }
}

//----------------------------------------------------------------//
const Chain* Miner::getBestBranch () const {

    return this->mBestBranch.get ();
}

//----------------------------------------------------------------//
size_t Miner::getLongestBranchSize () const {
    
    size_t max = 0;
    
    set < shared_ptr < Chain >>::const_iterator branchIt = this->mBranches.cbegin ();
    for ( ; branchIt != this->mBranches.cend (); ++branchIt ) {
        shared_ptr < const Chain > comp = *branchIt;
        
        size_t nBlocks = comp->countBlocks ();
        if ( max < nBlocks ) {
            max = nBlocks;
        }
    }
    return max;
}

//----------------------------------------------------------------//
bool Miner::getLazy () const {

    return this->mLazy;
}

//----------------------------------------------------------------//
const Ledger& Miner::getLedger () const {

    assert ( this->mBestBranch );
    return *this->mBestBranch;
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
bool Miner::hasBranch ( string miners ) const {

    set < shared_ptr < Chain >>::const_iterator branchIt = this->mBranches.cbegin ();
    for ( ; branchIt != this->mBranches.cend (); ++branchIt ) {
        shared_ptr < const Chain > branch = *branchIt;
        if ( branch->checkMiners ( miners )) return true;
    }
    return false;
}

//----------------------------------------------------------------//
void Miner::loadGenesis ( string path ) {

    fstream inStream;
    inStream.open ( path, ios_base::in );
    assert ( inStream.is_open ());

    Block block;
    FromJSONSerializer::fromJSON ( block, inStream );
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
    
    if ( this->mMinerID.size () == 0 ) {
        string keyID = this->mKeyPair.getKeyID ();
        const Ledger& ledger = this->getLedger ();
        shared_ptr < AccountKeyLookup > accountKeyLookup = ledger.getAccountKeyLookup ( keyID );
        if ( accountKeyLookup ) {
            this->mMinerID = ledger.getAccountName ( accountKeyLookup->mAccountIndex );
        }
    }
    assert ( this->mMinerID.size () > 0 );
}

//----------------------------------------------------------------//
Miner::Miner () :
    mLazy ( false ),
    mControlPermitted ( false ) {
    
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
bool Miner::pushTransaction ( shared_ptr < const Transaction > transaction ) {

    return this->TransactionQueue::pushTransaction ( transaction, transaction->control ( *this, *this->mBestBranch ));
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
void Miner::reset () {

    this->TransactionQueue::reset ();
    this->mBestBranch->reset ( 1 );
    this->mBranches.clear ();
    this->mBranches.insert ( this->mBestBranch );
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
void Miner::setChainRecorder ( shared_ptr < AbstractChainRecorder > chainRecorder ) {

    this->mChainRecorder = chainRecorder;
    if ( this->mChainRecorder ) {
        this->mChainRecorder->loadChain ( *this );
    }
}

//----------------------------------------------------------------//
void Miner::setGenesis ( const Block& block ) {
    
    this->mBranches.clear ();
    shared_ptr < Chain > chain = make_shared < Chain >();
    bool result = chain->pushBlock ( block );
    assert ( result );
    
    string identity = chain->getIdentity ();
    assert ( identity.size ());
    
    this->mBranches.insert ( chain );
    this->mBestBranch = chain;
}

//----------------------------------------------------------------//
void Miner::selectBranch () {
   
    shared_ptr < Chain > bestBranch;
   
    if ( this->mBranches.size ()) {
    
        time_t now = this->getTime ();
    
        set < shared_ptr < Chain >>::iterator branchIt = this->mBranches.begin ();
        bestBranch = *branchIt;
        
        for ( ++branchIt ; branchIt != this->mBranches.end (); ++branchIt ) {
            shared_ptr < Chain > comp = *branchIt;
            
            if ( Chain::compare ( *bestBranch, *comp, now ) > 0 ) {
                bestBranch = comp;
            }
        }
    }
    this->mBestBranch = bestBranch;
    assert ( this->mBestBranch );
}

//----------------------------------------------------------------//
Miner::SubmissionResponse Miner::submitBlock ( const Block& block ) {

    size_t blockHeight = block.getHeight ();
    
    assert ( blockHeight > 0 ); // TODO: handle this

    set < shared_ptr < Chain >>::iterator branchIt = this->mBranches.begin ();
    for ( ; branchIt != this->mBranches.end (); ++branchIt ) {
    
        shared_ptr < Chain > chain = *branchIt;
        size_t chainHeight = chain->countBlocks ();
        if ( blockHeight <= chainHeight ) {
            
            shared_ptr < Block > prevBlock = chain->getBlock ( blockHeight - 1 );
            assert ( prevBlock );
            if ( prevBlock->isParent ( block )) {
                
                if ( blockHeight == chainHeight ) {
                    chain->pushBlock ( block );
                }
                else {
                    shared_ptr < Block > rival = chain->getBlock ( blockHeight );
                    if ( block != *rival ) {
                        shared_ptr < Chain > fork = make_shared < Chain >( *chain );
                        fork->revert ( blockHeight - 1);
                        fork->pushVersion ();
                        fork->pushBlock ( block );
                        this->mBranches.insert ( fork );
                    }
                }
                return SubmissionResponse::ACCEPTED;
            }
        }
    }
    return SubmissionResponse::RESUBMIT_EARLIER;
}

//----------------------------------------------------------------//
void Miner::submitChain ( const Chain& chain ) {

    size_t nBlocks = chain.countBlocks ();
    if ( nBlocks > 1 ) {
        this->submitChainRecurse ( chain, nBlocks - 1 );
    }
}

//----------------------------------------------------------------//
void Miner::submitChainRecurse ( const Chain& chain, size_t blockID ) {

    shared_ptr < Block > block = chain.getBlock ( blockID );
    assert ( block );
    
    if ( blockID == 0 ) {
        // TODO: handle genesis block
        assert ( false );
        return;
    }
    
    SubmissionResponse response = this->submitBlock ( *block );
    if ( response == SubmissionResponse::RESUBMIT_EARLIER ) {
        this->submitChainRecurse ( chain, blockID - 1 );
        response = this->submitBlock ( *block );
    }
    assert ( response == SubmissionResponse::ACCEPTED );
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
