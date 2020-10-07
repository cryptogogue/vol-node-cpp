// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractChainRecorder.h>
#include <volition/Block.h>
#include <volition/Digest.h>
#include <volition/HTTPMiningMessenger.h>
#include <volition/MinerBase.h>
#include <volition/MinerLaunchTests.h>

namespace Volition {

//================================================================//
// RemoteMiner
//================================================================//

//----------------------------------------------------------------//
RemoteMiner::RemoteMiner () :
    mHeight ( 0 ),
    mForward ( true ) {
}

//----------------------------------------------------------------//
RemoteMiner::~RemoteMiner () {
}

//================================================================//
// MinerBase
//================================================================//

//----------------------------------------------------------------//
void MinerBase::affirmKey ( uint keyLength, unsigned long exp ) {

    if ( !this->mKeyPair ) {
        this->mKeyPair.rsa ( keyLength, exp );
    }
    assert ( this->mKeyPair );
}

//----------------------------------------------------------------//
void MinerBase::affirmMessenger () {

    if ( !this->mMessenger ) {
        this->mMessenger = make_shared < HTTPMiningMessenger >();
    }
}

//----------------------------------------------------------------//
void MinerBase::affirmVisage () {

    assert ( this->mKeyPair );
    this->mVisage = this->mKeyPair.sign ( this->mMotto, Digest::HASH_ALGORITHM_SHA256 );
}

//----------------------------------------------------------------//
bool MinerBase::checkBestBranch ( string miners ) const {

    assert ( this->mChain );
    return this->mChain->checkMiners ( miners );
}

//----------------------------------------------------------------//
bool MinerBase::controlPermitted () const {

    return ( this->mFlags & MINER_PERMIT_CONTROL );
}

//----------------------------------------------------------------//
void MinerBase::discoverMiners () {

    map < string, MinerInfo > miners = this->getChain ()->getMiners ();
        
    map < string, MinerInfo >::iterator minerIt = miners.begin ();
    for ( ; minerIt != miners.end (); ++minerIt ) {
        MinerInfo& minerInfo = minerIt->second;
        if ( minerIt->first != this->mMinerID ) {
            this->mRemoteMiners [ minerIt->first ].mURL = minerInfo.getURL (); // affirm
        }
    }
}

//----------------------------------------------------------------//
void MinerBase::extend ( time_t now ) {

    shared_ptr < Block > block = this->prepareBlock ( now );
    if ( block ) {
    
        this->pushBlock ( block );
        
        if ( this->mFlags & MINER_VERBOSE ) {
            LGN_LOG_SCOPE ( VOL_FILTER_ROOT, INFO, "WEB: WebMiner::runSolo () - step" );
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "WEB: height: %d", ( int )this->mChain->countBlocks ());
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "WEB.CHAIN: %s", this->mChain->printChain ().c_str ());
        }
        this->saveChain ();
        this->pruneTransactions ( *this->mChain );
    }
}

//----------------------------------------------------------------//
shared_ptr < const BlockTreeNode > MinerBase::getBestBranch () const {

    return this->mBestBranch;
}

//----------------------------------------------------------------//
const BlockTree& MinerBase::getBlockTree () const {

    return this->mBlockTree;
}

//----------------------------------------------------------------//
const Ledger* MinerBase::getChain () const {

    return this->mChain.get ();
}

//----------------------------------------------------------------//
const CryptoKey& MinerBase::getKeyPair () const {

    return this->mKeyPair;
}

//----------------------------------------------------------------//
Ledger& MinerBase::getLedger () {

    assert ( this->mChain );
    return *this->mChain;
}

//----------------------------------------------------------------//
const Ledger& MinerBase::getLedger () const {

    assert ( this->mChain );
    return *this->mChain;
}

//----------------------------------------------------------------//
string MinerBase::getMinerID () const {

    return this->mMinerID;
}

//----------------------------------------------------------------//
string MinerBase::getMotto () const {

    return this->mMotto;
}

//----------------------------------------------------------------//
BlockTreeNode::RewriteMode MinerBase::getRewriteMode () const {

    return this->mRewriteMode;
}

//----------------------------------------------------------------//
time_t MinerBase::getRewriteWindow () const {

    return this->mRewriteWindowInSeconds;
}

//----------------------------------------------------------------//
SerializableTime MinerBase::getStartTime () const {

    return this->mStartTime;
}

//----------------------------------------------------------------//
const Signature& MinerBase::getVisage () const {

    return this->mVisage;
}

//----------------------------------------------------------------//
bool MinerBase::isLazy () const {

    return ( this->mFlags & MINER_LAZY );
}

//----------------------------------------------------------------//
void MinerBase::loadGenesis ( string path ) {

    fstream inStream;
    inStream.open ( path, ios_base::in );
    assert ( inStream.is_open ());

    shared_ptr < Block > block = make_shared < Block >();
    FromJSONSerializer::fromJSON ( *block, inStream );
    this->setGenesis ( block );
}

//----------------------------------------------------------------//
void MinerBase::loadKey ( string keyfile, string password ) {
    UNUSED ( password );

    // TODO: password

    fstream inStream;
    inStream.open ( keyfile, ios_base::in );
    assert ( inStream.is_open ());
    
    Volition::FromJSONSerializer::fromJSON ( this->mKeyPair, inStream );
    assert ( this->mKeyPair );
}

//----------------------------------------------------------------//
MinerBase::MinerBase () :
    mFlags ( DEFAULT_FLAGS ),
    mRewriteMode ( BlockTreeNode::REWRITE_NONE ),
    mRewriteWindowInSeconds ( 0 ),
    mBlockVerificationPolicy ( Block::VerificationPolicy::ALL ) {
    
    MinerLaunchTests::checkEnvironment ();
}

//----------------------------------------------------------------//
MinerBase::~MinerBase () {
}

//----------------------------------------------------------------//
void MinerBase::permitControl ( bool permit ) {

    this->mFlags = SET_BITS ( this->mFlags, MINER_PERMIT_CONTROL, permit );
}

//----------------------------------------------------------------//
shared_ptr < Block > MinerBase::prepareBlock ( time_t now ) {
        
    shared_ptr < Block > prevBlock = this->mChain->getBlock ();
    assert ( prevBlock );
    
    shared_ptr < Block > block = make_shared < Block >( this->mMinerID, this->mVisage, now, prevBlock.get (), this->mKeyPair );
    this->fillBlock ( *this->mChain, *block );
    
    if ( !( this->isLazy () && ( block->countTransactions () == 0 ))) {
        block->sign ( this->mKeyPair, Digest::DEFAULT_HASH_ALGORITHM );
        return block;
    }
    return NULL;
}

//----------------------------------------------------------------//
void MinerBase::pushBlock ( shared_ptr < const Block > block ) {

    bool result = this->mChain->pushBlock ( *block, this->mBlockVerificationPolicy );
    assert ( result );
    
    BlockTreeNode::ConstPtr node = this->mBlockTree.affirmBlock ( block );
    assert ( node );
    
    if ( this->mChainTag == this->mBestBranch ) {
        this->mBestBranch = node;
    }
    this->mChainTag = node;
}

//----------------------------------------------------------------//
void MinerBase::reset () {

    this->TransactionQueue::reset ();
    this->mChain->reset ( 1 );
    this->mChain->clearSchemaCache ();
    if ( this->mChainRecorder ) {
        this->mChainRecorder->reset ();
    }
    this->Miner_reset ();
}

//----------------------------------------------------------------//
void MinerBase::saveChain () {

    if ( this->mChainRecorder ) {
        this->mChainRecorder->saveChain ( *this );
    }
}

//----------------------------------------------------------------//
void MinerBase::setChainRecorder ( shared_ptr < AbstractChainRecorder > chainRecorder ) {

    this->mChainRecorder = chainRecorder;
    if ( this->mChainRecorder ) {
        this->mChainRecorder->loadChain ( *this );
    }
}

//----------------------------------------------------------------//
void MinerBase::setGenesis ( shared_ptr < const Block > block ) {
    
    this->mChainTag = NULL;
    this->mBestBranch = NULL;
    
    assert ( block );
    
    shared_ptr < Ledger > chain = make_shared < Ledger >();
    this->mChain = chain;
    
    this->pushBlock ( block );
}

//----------------------------------------------------------------//
void MinerBase::setKeyPair ( const CryptoKey& key ) {

    this->mKeyPair = key;
}

//----------------------------------------------------------------//
void MinerBase::setLazy ( bool lazy ) {

    this->mFlags = SET_BITS ( this->mFlags, MINER_LAZY, lazy );
}

//----------------------------------------------------------------//
void MinerBase::setMessenger ( shared_ptr < AbstractMiningMessenger > messenger ) {

    this->mMessenger = messenger;
}

//----------------------------------------------------------------//
void MinerBase::setMinerID ( string minerID ) {

    this->mMinerID = minerID;
}

//----------------------------------------------------------------//
void MinerBase::setMotto ( string motto ) {

    this->mMotto = motto;
}

//----------------------------------------------------------------//
void MinerBase::setRewriteMode ( BlockTreeNode::RewriteMode mode ) {

    this->mRewriteMode = mode;
}

//----------------------------------------------------------------//
void MinerBase::setRewriteWindow ( time_t window ) {

    this->mRewriteWindowInSeconds = window;
    this->setRewriteMode ( BlockTreeNode::REWRITE_WINDOW );
}

//----------------------------------------------------------------//
void MinerBase::setVerbose ( bool verbose ) {

    this->mFlags = SET_BITS ( this->mFlags, MINER_VERBOSE, verbose );
}

//----------------------------------------------------------------//
void MinerBase::shutdown ( bool kill ) {

    this->Miner_shutdown ( kill );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void MinerBase::AbstractMiningMessengerClient_receiveBlock ( const MiningMessengerRequest& request, shared_ptr < const Block > block ) {
    
    unique_ptr < BlockQueueEntry > blockQueueEntry = make_unique < BlockQueueEntry >();
    blockQueueEntry->mRequest           = request;
    blockQueueEntry->mBlock             = block;

    Poco::ScopedLock < Poco::Mutex > chainMutexLock ( this->mMutex );
    this->mBlockQueue.push_back ( move ( blockQueueEntry ));
}

//----------------------------------------------------------------//
void MinerBase::AbstractMiningMessengerClient_receiveHeaders ( const MiningMessengerRequest& request, const list < shared_ptr < const BlockHeader >>& headers ) {
    
    unique_ptr < BlockQueueEntry > blockQueueEntry = make_unique < BlockQueueEntry >();
    blockQueueEntry->mRequest           = request;
    blockQueueEntry->mHeaders           = headers;

    Poco::ScopedLock < Poco::Mutex > chainMutexLock ( this->mMutex );
    this->mBlockQueue.push_back ( move ( blockQueueEntry ));
}

//----------------------------------------------------------------//
void MinerBase::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    UNUSED ( serializer );
    
//    serializer.serialize ( "chain", this->mChain );
}

//----------------------------------------------------------------//
void MinerBase::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
    UNUSED ( serializer );

//    serializer.serialize ( "chain", this->mChain );
}

//----------------------------------------------------------------//
void MinerBase::Miner_reset () {
}


//----------------------------------------------------------------//
void MinerBase::Miner_shutdown ( bool kill ) {
    UNUSED ( kill );

    // explicitly release messenger and possibly trigger shutdown
    this->mMessenger = NULL;
}

} // namespace Volition
