// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AccountODBM.h>
#include <volition/Asset.h>
#include <volition/AssetMethod.h>
#include <volition/AssetMethodInvocation.h>
#include <volition/AssetODBM.h>
#include <volition/BlockODBM.h>
#include <volition/PayoutPolicy.h>
#include <volition/Format.h>
#include <volition/Ledger.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/LuaContext.h>
#include <volition/MiningReward.h>
#include <volition/MonetaryPolicy.h>
#include <volition/Transaction.h>
#include <volition/TransactionFeeSchedule.h>
#include <volition/UnsecureRandom.h>

namespace Volition {

//================================================================//
// AbstractLedger
//================================================================//

//----------------------------------------------------------------//
AbstractLedger::AbstractLedger () {
}

//----------------------------------------------------------------//
AbstractLedger::~AbstractLedger () {
}

//----------------------------------------------------------------//
bool AbstractLedger::canReward ( string rewardName ) const {

    const Schema& schema = this->getSchema ();

    const MiningReward* reward = schema.getRewardOrNull ( rewardName );
    if ( !reward ) return false;
    
    LedgerFieldODBM < u64 > rewardCountField ( *this, AbstractLedger::keyFor_rewardCount ( rewardName ));
    u64 rewardCount = rewardCountField.get ( 0 );
    
    return ( rewardCount < reward->mQuantity );
}

//----------------------------------------------------------------//
bool AbstractLedger::checkMiners ( string miners ) const {
    
    const char* delim = ",";
    
    size_t start;
    size_t end = 0;
    
    size_t blockID = 0;
    while (( start = miners.find_first_not_of ( delim, end )) != std::string::npos ) {
        end = miners.find ( delim, start );
        string minerID = miners.substr ( start, end - start );
        shared_ptr < const Block > block = this->getBlock ( blockID );
        if ( !block ) return false;
        if (( blockID > 0 ) && ( block->getMinerID () != minerID )) return false;
        blockID++;
    }
    return true;
}

//----------------------------------------------------------------//
LedgerResult AbstractLedger::checkSchemaMethodsAndRewards ( const Schema& schema ) const {

    string out;

    LuaContext lua ( *this, 0 );

    Schema::Methods::const_iterator methodIt = schema.mMethods.cbegin ();
    for ( ; methodIt != schema.mMethods.cend (); ++methodIt ) {
        const AssetMethod& method = methodIt->second;
        LedgerResult result = lua.compile ( method.mLua );
        if ( !result ) {
        
            Format::write ( out, "LUA COMPILATION ERROR IN CRAFTING METHOD %s:\n", methodIt->first.c_str ());
            out.append ( result.getMessage ());
            out.append ( "\n" );
        }
    }
    
    Schema::Rewards::const_iterator rewardIt = schema.mRewards.cbegin ();
    for ( ; rewardIt != schema.mRewards.cend (); ++rewardIt ) {
        const MiningReward& reward = rewardIt->second;
        LedgerResult result = lua.compile ( reward.mLua );
        if ( !result ) {
        
            Format::write ( out, "LUA COMPILATION ERROR IN MINING REWARD %s:\n", rewardIt->first.c_str ());
            out.append ( result.getMessage ());
            out.append ( "\n" );
        }
    }
    
    return out;
}

//----------------------------------------------------------------//
string AbstractLedger::chooseReward ( string rewardName ) {

    if ( !this->canReward ( rewardName )) {
    
        set < string > available;
    
        const Schema& schema = this->getSchema ();
        Schema::Rewards::const_iterator rewardIt = schema.mRewards.begin ();
        for ( ; rewardIt != schema.mRewards.end (); ++rewardIt ) {
            
            string checkName = rewardIt->first;
            if ( checkName == rewardName ) continue;
            
            if ( this->canReward ( checkName )) {
                available.insert ( checkName );
            }
        }
        
        if ( available.size () == 0 ) return "";
        rewardName = UnsecureRandom::get ().randomDraw ( available );
    }

    return rewardName;
}

//----------------------------------------------------------------//
void AbstractLedger::clearSchemaCache () {

    this->mSchemaCache = NULL;
}

//----------------------------------------------------------------//
u64 AbstractLedger::countBlocks () const {

    return this->getHeight ();
}

//----------------------------------------------------------------//
u64 AbstractLedger::countVOL () const {

    return LedgerFieldODBM < u64 >( *this, AbstractLedger::keyFor_totalVOL ()).get ( 0 );
}

//----------------------------------------------------------------//
u64 AbstractLedger::createVOL ( u64 rewards, u64 prizes ) {

    LedgerFieldODBM < u64 > rewardPoolField ( *this, AbstractLedger::keyFor_rewardPool ());
    rewardPoolField.set ( rewardPoolField.get ( 0 ) + rewards );
    
    LedgerFieldODBM < u64 > prizePoolField ( *this, AbstractLedger::keyFor_prizePool ());
    rewardPoolField.set ( prizePoolField.get ( 0 ) + rewards );

    u64 total = rewards + prizes;

    LedgerFieldODBM < u64 > totalVOLField ( *this, AbstractLedger::keyFor_totalVOL ());
    totalVOLField.set ( totalVOLField.get ( 0 ) + total );
    
    return total;
}

//----------------------------------------------------------------//
shared_ptr < const Block > AbstractLedger::getBlock () const {

    u64 totalBlocks = this->countBlocks ();
    if ( !totalBlocks ) return NULL;

    return this->getBlock ( totalBlocks - 1 );
}

//----------------------------------------------------------------//
shared_ptr < const Block > AbstractLedger::getBlock ( u64 height ) const {

    BlockODBM blockODBM ( *this, height );
    return blockODBM.mBlock.get ();
}

//----------------------------------------------------------------//
shared_ptr < const Block > AbstractLedger::getBlock ( string hash ) const {

    LedgerKey KEY_FOR_BLOCK_HEIGHT_BY_HASH = keyFor_blockHeightByHash ( hash );

    if ( this->hasKey ( KEY_FOR_BLOCK_HEIGHT_BY_HASH )) {
        u64 height = this->getValue < u64 >( KEY_FOR_BLOCK_HEIGHT_BY_HASH );
        return this->getBlock ( height );
    }
    return NULL;
}

//----------------------------------------------------------------//
time_t AbstractLedger::getBlockDelayInSeconds () const {

    return ( time_t )this->getValue < u64 >( keyFor_blockDelay ());
}

//----------------------------------------------------------------//
Entropy AbstractLedger::getEntropy () const {

    return Entropy ( this->getEntropyString ());
}

//----------------------------------------------------------------//
string AbstractLedger::getEntropyString () const {

    return this->getValueOrFallback < string >( keyFor_entropy (), "" );
}

//----------------------------------------------------------------//
string AbstractLedger::getGenesisHash () const {
    
    BlockODBM genesisODBM ( *this, 0 );
    return genesisODBM ? genesisODBM.mHash.get () : "";
}

//----------------------------------------------------------------//
shared_ptr < const BlockHeader > AbstractLedger::getHeader ( u64 height ) const {

    BlockODBM blockODBM ( *this, height );
    return blockODBM.mHeader.get ();
}

//----------------------------------------------------------------//
u64 AbstractLedger::getHeight () const {

    return ( u64 )this->getVersion ();
}

//----------------------------------------------------------------//
string AbstractLedger::getIdentity () const {

    return this->getValueOrFallback < string >( keyFor_identity (), "" );
}

//----------------------------------------------------------------//
u64 AbstractLedger::getMaxBlockWeight () const {

    return this->getValue < u64 >( keyFor_maxBlockWeight ());
}

//----------------------------------------------------------------//
MonetaryPolicy AbstractLedger::getMonetaryPolicy () const {
        
    shared_ptr < MonetaryPolicy > monetaryPolicy = this->getObjectOrNull < MonetaryPolicy >( keyFor_monetaryPolicy ());
    return monetaryPolicy ? *monetaryPolicy : MonetaryPolicy ();
}

//----------------------------------------------------------------//
PayoutPolicy AbstractLedger::getPayoutPolicy () const {
    
    shared_ptr < PayoutPolicy > feeDistributionTable = this->getObjectOrNull < PayoutPolicy >( keyFor_payoutPolicy ());
    return feeDistributionTable ? *feeDistributionTable : PayoutPolicy ();
}

//----------------------------------------------------------------//
u64 AbstractLedger::getPayoutPool () const {

    return this->getValueOrFallback < u64 >( keyFor_payoutPool (), 0 );
}

//----------------------------------------------------------------//
u64 AbstractLedger::getPrizePool () const {

    return this->getValueOrFallback < u64 >( keyFor_prizePool (), 0 );
}

//----------------------------------------------------------------//
u64 AbstractLedger::getRewardPool () const {

    return this->getValueOrFallback < u64 >( keyFor_rewardPool (), 0 );
}

//----------------------------------------------------------------//
time_t AbstractLedger::getRewriteWindowInSeconds () const {

    return ( time_t )this->getValue < u64 >( keyFor_rewriteWindow ());
}

//----------------------------------------------------------------//
const Schema& AbstractLedger::getSchema () const {

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    if ( !this->mSchemaCache ) {
        this->mSchemaCache = make_shared < map < string, shared_ptr < const Schema >>>();
    }
    map < string, shared_ptr < const Schema >>& schemaCache = *this->mSchemaCache;
    
    string schemaHash = this->getSchemaHash ();
    
    map < string, shared_ptr < const Schema >>::const_iterator schemaIt = schemaCache.find ( schemaHash );
    if ( schemaIt != schemaCache.cend ()) {
        LGN_LOG ( VOL_FILTER_LEDGER, INFO, "Found in cache" );
        return *schemaIt->second;
    }
    
    LGN_LOG ( VOL_FILTER_LEDGER, INFO, "Loading schema" );

    shared_ptr < Schema > schema = make_shared < Schema >();
    schemaCache [ schemaHash ] = schema;

    string schemaString = this->getSchemaString ();
    if ( schemaString.size () > 0 ) {
        FromJSONSerializer::fromJSONString ( *schema, schemaString );
    }
    return *schema;
}

//----------------------------------------------------------------//
string AbstractLedger::getSchemaHash () const {

    return this->getValueOrFallback < string >( keyFor_schemaHash (), "" );
}

//----------------------------------------------------------------//
string AbstractLedger::getSchemaString () const {

    return this->getValue < string >( keyFor_schema ());
}

//----------------------------------------------------------------//
SchemaVersion AbstractLedger::getSchemaVersion () const {

    SchemaVersion schemaVersion;
    const string schemaVersionString = this->getValueOrFallback < string >( keyFor_schemaVersion (), "" );
    if ( schemaVersionString.size () > 0 ) {
        FromJSONSerializer::fromJSONString ( schemaVersion, schemaVersionString );
    }
    return schemaVersion;
}

//----------------------------------------------------------------//
TransactionFeeSchedule AbstractLedger::getTransactionFeeSchedule () const {
        
    shared_ptr < TransactionFeeSchedule > feeSchedule = this->getObjectOrNull < TransactionFeeSchedule >( keyFor_transactionFeeSchedule ());
    return feeSchedule ? *feeSchedule : TransactionFeeSchedule ();
}

//----------------------------------------------------------------//
UnfinishedBlockList AbstractLedger::getUnfinished () {

    shared_ptr < UnfinishedBlockList > unfinished = this->getObjectOrNull < UnfinishedBlockList >( keyFor_unfinished ());
    return unfinished ? *unfinished : UnfinishedBlockList ();
}

//----------------------------------------------------------------//
bool AbstractLedger::hasBlock ( string hash ) const {

    return this->hasKey ( keyFor_blockHeightByHash ( hash ) );
}

//----------------------------------------------------------------//
bool AbstractLedger::hasTransaction ( string accountName, string uuid ) const {

    AccountID accountID = this->getAccountID ( accountName );
    if ( accountID != AccountID::NULL_INDEX ) {
        return AccountODBM ( this->getLedger (), accountID ).getTransactionLookupField ( uuid ).exists ();
    }
    return false;
}

//----------------------------------------------------------------//
void AbstractLedger::init () {

    this->revertAndClear ( 0 );
    this->setObject < SerializableSet < string >>( keyFor_miners (), SerializableSet < string > ());
    this->setValue < AssetID::Index >( keyFor_globalAccountCount (), 0 );
    this->setValue < AssetID::Index >( keyFor_globalAssetCount (), 0 );
    this->setValue < string >( keyFor_schema (), "{}" );
    this->setObject < TransactionFeeSchedule >( keyFor_transactionFeeSchedule (), TransactionFeeSchedule ());
}

//----------------------------------------------------------------//
LedgerResult AbstractLedger::invoke ( string accountName, const AssetMethodInvocation& invocation, time_t time ) {

    const Schema& schema = this->getSchema ();

    const AssetMethod* method = schema.getMethodOrNull ( invocation.mMethodName );
    if ( !( method && ( method->mWeight == invocation.mWeight ) && ( method->mMaturity == invocation.mMaturity ))) return false;

    // make sure account exists
    AccountID accountID = this->getAccountID ( accountName );
    if ( accountID == AccountID::NULL_INDEX ) return false;

    // TODO: this is brutally inefficient, but we're doing it for now. can add a cache of LuaContext objects later to speed things up.
    LuaContext lua ( *this, time );
    return lua.invoke ( accountName, *method, invocation );
}

//----------------------------------------------------------------//
bool AbstractLedger::isGenesis () const {

    return ( this->getHeight () == 0 );
}

//----------------------------------------------------------------//
LedgerResult AbstractLedger::invokeReward ( string minerID, string rewardName, time_t time ) {

    if ( !this->canReward ( rewardName )) return true;

    LuaContext lua ( *this, time );
    return lua.invoke ( minerID, rewardName );
}

//----------------------------------------------------------------//
void AbstractLedger::payout ( u64 amount ) {

    if ( amount == 0 ) return;
    this->setPayoutPool ( this->getPayoutPool () + amount );
    
    shared_ptr < PayoutPolicy > distributionTable = this->getObjectOrNull < PayoutPolicy >( this->keyFor_payoutPolicy ());
    if ( distributionTable ) {
        distributionTable->payout ( *this );
    }
}

//----------------------------------------------------------------//
string AbstractLedger::printChain ( const char* pre, const char* post ) const {

    string str;

    if ( pre ) {
        Format::write ( str, "%s", pre );
    }

    size_t nBlocks = this->countBlocks ();
    if ( nBlocks ) {
    
        Format::write ( str, "-" );
    
        for ( size_t i = 1; i < nBlocks; ++i ) {
            shared_ptr < const Block > block = this->getBlock ( i );
            assert ( block );
            Format::write ( str, ",%s", block->mMinerID.c_str ());
        }
    }

    if ( post ) {
        Format::write ( str, "%s", post );
    }
    
    return str;
}

//----------------------------------------------------------------//
LedgerResult AbstractLedger::pushBlock ( const Block& block, Block::VerificationPolicy policy ) {

    // make sure there's a current schema cache
    this->getSchema ();

    Ledger fork;
    fork.takeSnapshot ( *this );
    fork.mSchemaCache = this->mSchemaCache; // TODO: this this later

    LedgerResult result = block.apply ( fork, policy );

    if ( result ) {
    
        fork.setValue < u64 >( keyFor_blockHeightByHash ( block.getDigest ().toHex ()), block.getHeight ());
        fork.pushVersion ();
        
        this->takeSnapshot ( fork );
    }
    return result;
}

//----------------------------------------------------------------//
void AbstractLedger::serializeEntitlements ( const Account& account, AbstractSerializerTo& serializer ) const {

    serializer.context ( "account", [ & ]( AbstractSerializerTo& serializer ) {

        serializer.serialize ( "policy", this->getEntitlements < AccountEntitlements >( account.mPolicy ));
        
        if ( account.mBequest ) {
            serializer.serialize ( "bequest", this->getEntitlements < AccountEntitlements >( *account.mBequest ));
        }
    });
    
    serializer.context ( "keys", [ & ]( AbstractSerializerTo& serializer ) {

        SerializableMap < string, KeyAndPolicy >::const_iterator keysIt = account.mKeys.cbegin ();
        for ( ; keysIt != account.mKeys.cend (); ++keysIt ) {

            string keyName = keysIt->first;
            const KeyAndPolicy& keyAndPolicy = keysIt->second;

            serializer.context ( keysIt->first, [ & ]( AbstractSerializerTo& serializer ) {

                serializer.serialize ( "policy", this->getEntitlements < KeyEntitlements >( keyAndPolicy.mPolicy ));
                
                if ( account.mBequest ) {
                    serializer.serialize ( "bequest", this->getEntitlements < KeyEntitlements >( *keyAndPolicy.mBequest ));
                }
            });
        }
    });
}

//----------------------------------------------------------------//
void AbstractLedger::setEntitlements ( string name, const Entitlements& entitlements ) {

    LedgerKey KEY_FOR_ENTITLEMENTS = keyFor_entitlements ( name );
    this->setObject < Entitlements >( KEY_FOR_ENTITLEMENTS, entitlements );
}

//----------------------------------------------------------------//
void AbstractLedger::setEntropyString ( string entropy ) {

    this->setValue < string >( keyFor_entropy (), entropy );
}

//----------------------------------------------------------------//
bool AbstractLedger::setIdentity ( string identity ) {

    LedgerKey KEY_FOR_IDENTITY = keyFor_identity ();
    if ( this->hasValue ( KEY_FOR_IDENTITY )) return false;
    this->setValue < string >( KEY_FOR_IDENTITY, identity );
    return true;
}

//----------------------------------------------------------------//
void AbstractLedger::setMonetaryPolicy ( const MonetaryPolicy& monetaryPolicy ) {

    this->setObject < MonetaryPolicy >( keyFor_monetaryPolicy (), monetaryPolicy );
}

//----------------------------------------------------------------//
LedgerResult AbstractLedger::setPayoutPolicy ( const PayoutPolicy& distributionTable ) {

    if ( !distributionTable.isBalanced ()) return "Distribution table does not balance.";
    if ( !distributionTable.hasAccounts ( *this )) return "Distribution table names unknown accounts.";

    this->setObject < PayoutPolicy >( keyFor_payoutPolicy (), distributionTable );
    return true;
}

//----------------------------------------------------------------//
void AbstractLedger::setPayoutPool ( u64 amount ) {

    this->setValue < u64 >( keyFor_payoutPool (), amount );
}

//----------------------------------------------------------------//
void AbstractLedger::setSchema ( const Schema& schema ) {

    string schemaHash = Digest ( schema, Digest::HASH_ALGORITHM_MD5 ).toHex ();

    this->setObject < Schema >( AbstractLedger::keyFor_schema (), schema );
    this->setObject < SchemaVersion >( AbstractLedger::keyFor_schemaVersion (), schema.mVersion );
    this->setValue < string >( AbstractLedger::keyFor_schemaHash (), schemaHash );
    
    if ( !this->mSchemaCache ) {
        this->mSchemaCache = make_shared < map < string, shared_ptr < const Schema >>>();
    }
    ( *this->mSchemaCache )[ schemaHash ] = make_shared < Schema >( schema );
}

//----------------------------------------------------------------//
void AbstractLedger::setTransactionFeeSchedule ( const TransactionFeeSchedule& feeSchedule ) {

    this->setObject < TransactionFeeSchedule >( keyFor_transactionFeeSchedule (), feeSchedule );
}

//----------------------------------------------------------------//
void AbstractLedger::setUnfinished ( const UnfinishedBlockList& unfinished ) {

    this->setObject < UnfinishedBlockList >( keyFor_unfinished (), unfinished );
}

//----------------------------------------------------------------//
bool AbstractLedger::verify ( const AssetMethodInvocation& invocation ) {

    const Schema& schema = this->getSchema ();
    const AssetMethod* method = schema.getMethodOrNull ( invocation.mMethodName );
    return ( method && ( method->mWeight == invocation.mWeight ) && ( method->mMaturity == invocation.mMaturity ));
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
AbstractLedger& AbstractLedger::AbstractLedgerComponent_getLedger () {

    return *this;
}

//----------------------------------------------------------------//
const AbstractLedger& AbstractLedger::AbstractLedgerComponent_getLedger () const {

    return *this;
}

//----------------------------------------------------------------//
void AbstractLedger::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    this->init ();
    
    SerializableVector < Block > blocks;
    serializer.serialize ( "blocks", blocks );

    size_t size = blocks.size ();
    for ( size_t i = 0; i < size; ++i ) {
    
        Block block = blocks [ i ];
        LedgerResult result = this->pushBlock ( block, Block::VerificationPolicy::NONE );
        assert ( result );
    }
}

//----------------------------------------------------------------//
void AbstractLedger::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    SerializableVector < Block > blocks;
    
    u64 totalBlocks = this->countBlocks ();
    for ( u64 i = 0; i < totalBlocks; ++i ) {
        shared_ptr < const Block > block = this->getBlock ( i );
        assert ( block );
        blocks.push_back ( *block );
    }
    serializer.serialize ( "blocks", blocks );
}

} // namespace Volition
