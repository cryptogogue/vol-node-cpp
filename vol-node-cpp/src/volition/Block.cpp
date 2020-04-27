// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Format.h>
#include <volition/TheContext.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {

//================================================================//
// Block
//================================================================//

//----------------------------------------------------------------//
void Block::affirmHash () {

    if ( !this->mSignature.getDigest ()) {
        this->sign ( CryptoKey ());
    }
}

//----------------------------------------------------------------//
bool Block::apply ( Ledger& ledger ) const {

    if ( ledger.getVersion () != this->mHeight ) return false;
    if ( !this->verify ( ledger )) return false;

    // some transactions need to be applied later.
    // we need to evaluate if they are legal now.
    // then process them once we have the entropy.
    
    // before applying the block, we need to apply the entropy.
    // then, get the list of blocks with transactions due on the current version.
    // apply those transactions and remove them from the pending list.
    // then, as we push the block, if it has pending transactions, add them.
    // if a block is removed or added from the list, flag it.
    // if it's been flagged, record it in the ledger at the end.

    // apply the entropy up front.
    this->applyEntropy ( ledger );

    // process unfinished blocks.
    UnfinishedBlockList unfinished = ledger.getUnfinished ();
    bool unfinishedChanged = false;
    
    UnfinishedBlockList nextUnfinished;
    UnfinishedBlockList::Iterator unfinishedBlockIt = unfinished.mBlocks.cbegin ();
    for ( ; unfinishedBlockIt != unfinished.mBlocks.end (); ++unfinishedBlockIt ) {
        UnfinishedBlock unfinishedBlock = *unfinishedBlockIt;
        
        if ( unfinishedBlock.mMaturity == this->mHeight ) {
            
            shared_ptr < Block > block = ledger.getBlock ( unfinishedBlock.mBlockID );
            assert ( block );
            
            size_t nextMaturity = block->applyTransactions ( ledger );
            
            if ( nextMaturity > this->mHeight ) {
            
                unfinishedBlock.mMaturity = nextMaturity;
                nextUnfinished.mBlocks.push_back ( unfinishedBlock );
            }
            
            unfinishedChanged = true;
        }
    }

    // apply transactions
    size_t nextMaturity = this->applyTransactions ( ledger );
    
    if ( nextMaturity > this->mHeight ) {
    
        UnfinishedBlock unfinishedBlock;
        unfinishedBlock.mBlockID = this->mHeight;
        unfinishedBlock.mMaturity = nextMaturity;
        nextUnfinished.mBlocks.push_back ( unfinishedBlock );
        
        unfinishedChanged = true;
    }
    
    // check pending block list, and apply if changed.
    if ( unfinishedChanged ) {
        ledger.setUnfinished ( nextUnfinished );
    }
    
    ledger.setBlock ( *this );
    
    return true;
}

//----------------------------------------------------------------//
size_t Block::applyTransactions ( Ledger& ledger ) const {

    size_t nextMaturity = this->mHeight;
    size_t height = ledger.getVersion ();

    SchemaHandle schemaHandle ( ledger );

    shared_ptr < const Account > miner = ledger.getAccount ( this->mMinerID );
    assert ( miner || ledger.isGenesis ());

    size_t gratuity = 0;

    if ( ledger.getVersion () >= this->mHeight ) {
        
        // apply block transactions.
        for ( size_t i = 0; i < this->mTransactions.size (); ++i ) {
            const Transaction& transaction = *this->mTransactions [ i ];
            
            size_t transactionMaturity = this->mHeight + transaction.maturity ();
            if ( transactionMaturity == height ) {
                TransactionResult result = transaction.apply ( ledger, schemaHandle );
                assert ( result );
                gratuity += transaction.getGratuity ();
            }
            
            if ( nextMaturity < transactionMaturity ) {
                nextMaturity = transactionMaturity;
            }
        }
    }
    
    if ( miner && ( gratuity > 0 )) {
        Account minerUpdated = *miner;
        minerUpdated.mBalance += gratuity;
        ledger.setAccount ( minerUpdated );
    }
    return nextMaturity;
}

//----------------------------------------------------------------//
void Block::applyEntropy ( Ledger& ledger ) const {

    ledger.setEntropyString ( this->mAllure.toString ());
}

//----------------------------------------------------------------//
Block::Block () :
    mHeight ( 0 ),
    mTime ( 0 ) {
}

//----------------------------------------------------------------//
Block::Block ( string minerID, time_t now, const Block* prevBlock, const CryptoKey& key, string hashAlgorithm ) :
    mMinerID ( minerID ),
    mHeight ( 0 ),
    mTime ( now ) {
    
    if ( prevBlock ) {
        
        this->mHeight = prevBlock->mHeight + 1;
        this->mPrevDigest = prevBlock->mSignature.getDigest ();
        
        // TODO: allure should be generated using an RFC6979 deterministic signature.
        // use a sample hash for now.
        Poco::Crypto::ECDSADigestEngine signature ( key, hashAlgorithm );
        this->computeAllure ( signature );
        //this->mAllure = signature.signature ();
        this->mAllure = signature.digest ();
    }
}

//----------------------------------------------------------------//
Block::~Block () {
}

//----------------------------------------------------------------//
int Block::compare ( const Block& block0, const Block& block1 ) {

    assert ( block0.mHeight == block1.mHeight );

    if ( block0 != block1 ) {
        
        if ( TheContext::get ().getScoringMode () == TheContext::ScoringMode::ALLURE ) {
        
            string allure0 = Poco::DigestEngine::digestToHex ( block0.mAllure );
            string allure1 = Poco::DigestEngine::digestToHex ( block1.mAllure );
            
//            printf ( "allure0: %s\n", allure0.c_str ());
//            printf ( "allure1: %s\n", allure1.c_str ());
            
            assert ( allure0 != allure1 );
            
            return allure0.compare ( allure1 ) < 0 ? -1 : 1;
        }
        else {
        
            size_t score0 = block0.getScore ();
            size_t score1 = block1.getScore ();
            
            if ( score0 < score1 ) return -1;
            if ( score0 > score1 ) return 1;
        }
    }
    return 0;
}

//----------------------------------------------------------------//
void Block::computeAllure ( Poco::Crypto::ECDSADigestEngine& signature ) const {

    Poco::DigestOutputStream signatureStream ( signature );
    signatureStream << this->mMinerID;
    signatureStream << this->mHeight;
    signatureStream << this->mPrevDigest;
    signatureStream.close ();
}

//----------------------------------------------------------------//
size_t Block::countTransactions () const {

    return this->mTransactions.size ();
}

//----------------------------------------------------------------//
string Block::getHash () const {

    return this->mSignature.getDigest ().toString ();
}

//----------------------------------------------------------------//
size_t Block::getHeight () const {

    return this->mHeight;
}

//----------------------------------------------------------------//
const Signature& Block::getSignature () const {

    return this->mSignature;
}

//----------------------------------------------------------------//
string Block::getMinerID () const {

    return this->mMinerID;
}

//----------------------------------------------------------------//
size_t Block::getScore () const {

//    if ( TheContext::get ().getScoringMode () == TheContext::ScoringMode::ALLURE ) {
//        string allureString = Poco::DigestEngine::digestToHex ( this->mAllure );
//        return std::hash < string >{}( allureString );
//    }
    
    if ( this->mHeight == 0 ) return 0;
    
    size_t modulo = TheContext::get ().getScoringModulo ();
    size_t height = ( this->mHeight - 1 ) % modulo;
    size_t minerID = ( size_t )strtol ( this->mMinerID.c_str (), 0, 10 );
    
    return height <= minerID ? minerID - height : (( modulo - height ) + minerID );
}

//----------------------------------------------------------------//
time_t Block::getTime () const {

    return this->mTime;
}

//----------------------------------------------------------------//
bool Block::isGenesis () const {

    return ( this->mHeight == 0 );
}

//----------------------------------------------------------------//
bool Block::isInRewriteWindow ( time_t now ) const {

    double window = TheContext::get ().getWindow ();
    double diff = difftime ( now, this->mTime );
    return diff < window;
}

//----------------------------------------------------------------//
bool Block::isParent ( const Block& block ) const {

    return ( this->mSignature.getDigest () == block.mPrevDigest );
}

//----------------------------------------------------------------//
void Block::pushTransaction ( shared_ptr < const Transaction > transaction ) {

    this->mTransactions.push_back ( transaction );
}

//----------------------------------------------------------------//
void Block::setAllure ( const Digest& allure ) {

    this->mAllure = allure;
}

//----------------------------------------------------------------//
void Block::setMinerID ( string minerID ) {

    this->mMinerID = minerID;
}

//----------------------------------------------------------------//
void Block::setPreviousBlock ( const Block& prevBlock ) {

    this->mHeight = prevBlock.mHeight + 1;
    this->mPrevDigest = prevBlock.mSignature.getDigest ();
}

//----------------------------------------------------------------//
const Digest& Block::sign ( const CryptoKey& key, string hashAlgorithm ) {
    
    this->mSignature = key.sign ( *this, hashAlgorithm );
    return this->mSignature.getSignature ();
}

//----------------------------------------------------------------//
bool Block::verify ( const Ledger& ledger ) const {

    shared_ptr < MinerInfo > minerInfo = ledger.getMinerInfo ( this->mMinerID );

    if ( minerInfo ) {
        return this->verify ( minerInfo->getPublicKey ());
    }

    // no miner info; must be the genesis block
    if ( this->mHeight > 0 ) return false; // genesis block must be height 0
    
    return true;
}

//----------------------------------------------------------------//
bool Block::verify ( const CryptoKey& key ) const {

//    if ( this->mHeight > 0 ) {
//
//        // verify allure
//        string hashAlgorithm = this->mSignature.getHashAlgorithm ();
//
//        Poco::Crypto::ECDSADigestEngine signature ( key, hashAlgorithm );
//        this->computeAllure ( signature );
//
//        if ( !signature.verify ( this->mAllure )) {
//            return false;
//        }
//    }
    return key.verify ( this->mSignature, *this );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Block::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    
    serializer.serialize ( "height",        this->mHeight );
    
    string iso8601;
    serializer.serialize ( "time", iso8601 );
    this->mTime = Format::fromISO8601 ( iso8601 );
    
    if ( !this->isGenesis ()) {
    
        serializer.serialize ( "minerID",       this->mMinerID );
        serializer.serialize ( "prevDigest",    this->mPrevDigest );
        serializer.serialize ( "allure",        this->mAllure );
    }
    
    serializer.serialize ( "signature",     this->mSignature );
    serializer.serialize ( "transactions",  this->mTransactions );
    
    this->affirmHash ();
}

//----------------------------------------------------------------//
void Block::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
    
    serializer.serialize ( "height",        this->mHeight );
    
    string iso8601 = Format::toISO8601 ( this->mTime );
    serializer.serialize ( "time", iso8601 );
    
     if ( !this->isGenesis ()) {
        
        serializer.serialize ( "minerID",       this->mMinerID );
        serializer.serialize ( "prevDigest",    this->mPrevDigest );
        serializer.serialize ( "allure",        this->mAllure );
    }
    
    if ( !serializer.isDigest ()) {
        serializer.serialize ( "signature",     this->mSignature );
    }
    
    serializer.serialize ( "transactions",  this->mTransactions );
}

} // namespace Volition
