// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AccountODBM.h>
#include <volition/Asset.h>
#include <volition/AssetMethod.h>
#include <volition/AssetMethodInvocation.h>
#include <volition/AssetODBM.h>
#include <volition/BlockODBM.h>
#include <volition/FeeDistributionTable.h>
#include <volition/FeeSchedule.h>
#include <volition/Format.h>
#include <volition/Ledger.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/LuaContext.h>
#include <volition/MiningReward.h>
#include <volition/Transaction.h>

namespace Volition {

//================================================================//
// Ledger
//================================================================//

//----------------------------------------------------------------//
bool Ledger::checkMiners ( string miners ) const {
    
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
LedgerResult Ledger::checkSchemaMethodsAndRewards ( const Schema& schema ) const {

    string out;

    LuaContext lua ( *this, 0 );

    Schema::Methods::const_iterator methodIt = schema.mMethods.cbegin ();
    for ( ; methodIt != schema.mMethods.cend (); ++methodIt ) {
        const AssetMethod& method = methodIt->second;
        LedgerResult result = lua.compile ( method.mLua );
        if ( !result ) {
        
            Format::write ( out, "LUA COMPILATION ERROR IN CRAFINT METHOD %s:\n", methodIt->first.c_str ());
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
void Ledger::clearSchemaCache () {

    this->mSchemaCache = NULL;
}

//----------------------------------------------------------------//
size_t Ledger::countBlocks () const {

    return LedgerFieldODBM < u64 >( *this, Ledger::keyFor_globalBlockCount ()).get ( 0 );
}

//----------------------------------------------------------------//
u64 Ledger::countVOL () const {

    return LedgerFieldODBM < u64 >( *this, Ledger::keyFor_VOL ()).get ( 0 );
}

//----------------------------------------------------------------//
u64 Ledger::createVOL ( u64 amount ) {

    LedgerFieldODBM < u64 > totalVOLField ( *this, Ledger::keyFor_VOL ());
    totalVOLField.set ( totalVOLField.get ( 0 ) + amount );
    return amount;
}

//----------------------------------------------------------------//
void Ledger::distribute ( u64 amount ) {

    if ( amount == 0 ) return;
    this->setFeeDistributionPool ( this->getFeeDistributionPool () + amount );
    
    shared_ptr < FeeDistributionTable > distributionTable = this->getObjectOrNull < FeeDistributionTable >( this->keyFor_feeDistributionTable ());
    if ( distributionTable ) {
        distributionTable->distribute ( *this );
    }
}

//----------------------------------------------------------------//
shared_ptr < const Block > Ledger::getBlock () const {

    u64 totalBlocks = this->countBlocks ();
    if ( !totalBlocks ) return NULL;

    return this->getBlock ( totalBlocks - 1 );
}

//----------------------------------------------------------------//
shared_ptr < const Block > Ledger::getBlock ( u64 height ) const {

    BlockODBM blockODBM ( *this, height );
    return blockODBM.mBlock.get ();
}

//----------------------------------------------------------------//
shared_ptr < const Block > Ledger::getBlock ( string hash ) const {

    LedgerKey KEY_FOR_BLOCK_HEIGHT_BY_HASH = keyFor_blockHeightByHash ( hash );

    if ( this->hasKey ( KEY_FOR_BLOCK_HEIGHT_BY_HASH )) {
        u64 height = this->getValue < u64 >( KEY_FOR_BLOCK_HEIGHT_BY_HASH );
        return this->getBlock ( height );
    }
    return NULL;
}

//----------------------------------------------------------------//
time_t Ledger::getBlockDelayInSeconds () const {

    return ( time_t )this->getValue < u64 >( keyFor_blockDelay ());
}

//----------------------------------------------------------------//
u64 Ledger::getBlockSizeInPoints () const {

    return this->getValue < u64 >( keyFor_blockSize ());
}

//----------------------------------------------------------------//
Entropy Ledger::getEntropy () const {

    return Entropy ( this->getEntropyString ());
}

//----------------------------------------------------------------//
string Ledger::getEntropyString () const {

    return this->getValueOrFallback < string >( keyFor_entropy (), "" );
}

//----------------------------------------------------------------//
u64 Ledger::getFeeDistributionPool () const {

    return this->getValueOrFallback < u64 >( keyFor_feeDistributionPool (), 0 );
}

//----------------------------------------------------------------//
FeeDistributionTable Ledger::getFeeDistributionTable () const {
    
    shared_ptr < FeeDistributionTable > feeDistributionTable = this->getObjectOrNull < FeeDistributionTable >( keyFor_feeDistributionTable ());
    return feeDistributionTable ? *feeDistributionTable : FeeDistributionTable ();
}

//----------------------------------------------------------------//
FeeSchedule Ledger::getFeeSchedule () const {
        
    shared_ptr < FeeSchedule > feeSchedule = this->getObjectOrNull < FeeSchedule >( keyFor_feeSchedule ());
    return feeSchedule ? *feeSchedule : FeeSchedule ();
}

//----------------------------------------------------------------//
string Ledger::getGenesisHash () const {
    
    BlockODBM genesisODBM ( *this, 0 );
    return genesisODBM ? genesisODBM.mHash.get () : "";
}

//----------------------------------------------------------------//
u64 Ledger::getHeight () const {

    return ( u64 )this->getVersion ();
}

//----------------------------------------------------------------//
string Ledger::getIdentity () const {

    return this->getValueOrFallback < string >( keyFor_identity (), "" );
}

//----------------------------------------------------------------//
time_t Ledger::getRewriteWindowInSeconds () const {

    return ( time_t )this->getValue < u64 >( keyFor_rewriteWindow ());
}

//----------------------------------------------------------------//
const Schema& Ledger::getSchema () {

    if ( !this->mSchemaCache ) {
        this->mSchemaCache = make_shared < map < string, Schema >>();
    }
    map < string, Schema >& schemaCache = *this->mSchemaCache;
    
    string schemaHash = this->getSchemaHash ();
    
    map < string, Schema >::const_iterator schemaIt = schemaCache.find ( schemaHash );
    if ( schemaIt != schemaCache.cend ()) {
        return schemaIt->second;
    }
    
    Schema& schema = schemaCache[ schemaHash ];
    string schemaString = this->getSchemaString ();
    if ( schemaString.size () > 0 ) {
        FromJSONSerializer::fromJSONString ( schema, schemaString );
    }
    return schema;
}

//----------------------------------------------------------------//
string Ledger::getSchemaHash () const {

    return this->getValueOrFallback < string >( keyFor_schemaHash (), "" );
}

//----------------------------------------------------------------//
string Ledger::getSchemaString () const {

    return this->getValue < string >( keyFor_schema ());
}

//----------------------------------------------------------------//
SchemaVersion Ledger::getSchemaVersion () const {

    SchemaVersion schemaVersion;
    const string schemaVersionString = this->getValueOrFallback < string >( keyFor_schemaVersion (), "" );
    if ( schemaVersionString.size () > 0 ) {
        FromJSONSerializer::fromJSONString ( schemaVersion, schemaVersionString );
    }
    return schemaVersion;
}

//----------------------------------------------------------------//
UnfinishedBlockList Ledger::getUnfinished () {

    shared_ptr < UnfinishedBlockList > unfinished = this->getObjectOrNull < UnfinishedBlockList >( keyFor_unfinished ());
    return unfinished ? *unfinished : UnfinishedBlockList ();
}

//----------------------------------------------------------------//
bool Ledger::hasTransaction ( string accountName, string uuid ) const {

    AccountID accountID = this->getAccountID ( accountName );
    if ( accountID != AccountID::NULL_INDEX ) {
        return AccountODBM ( this->getLedger (), accountID ).getTransactionLookupField ( uuid ).exists ();
    }
    return false;
}

//----------------------------------------------------------------//
void Ledger::init () {

    this->clear ();
    this->setObject < SerializableSet < string >>( keyFor_miners (), SerializableSet < string > ());
    this->setValue < AssetID::Index >( keyFor_globalAccountCount (), 0 );
    this->setValue < AssetID::Index >( keyFor_globalAssetCount (), 0 );
    this->setValue < string >( keyFor_schema (), "{}" );
    this->setObject < FeeSchedule >( keyFor_feeSchedule (), FeeSchedule ());
}

//----------------------------------------------------------------//
LedgerResult Ledger::invoke ( string accountName, const AssetMethodInvocation& invocation, time_t time ) {

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
bool Ledger::isGenesis () const {

    return ( this->getHeight () == 0 );
}

//----------------------------------------------------------------//
LedgerResult Ledger::invokeReward ( string minerID, string rewardName, time_t time ) {

    if ( !rewardName.size ()) return true;

    // TODO: this is brutally inefficient, but we're doing it for now. can add a cache of LuaContext objects later to speed things up.
    LuaContext lua ( *this, time );
    return lua.invoke ( minerID, rewardName );
}

//----------------------------------------------------------------//
Ledger::Ledger () {

    this->init ();
}

//----------------------------------------------------------------//
Ledger::Ledger ( Ledger& other ) :
    VersionedStore ( other ) {
}

//----------------------------------------------------------------//
Ledger::~Ledger () {
}

//----------------------------------------------------------------//
string Ledger::printChain ( const char* pre, const char* post ) const {

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
bool Ledger::pushBlock ( const Block& block, Block::VerificationPolicy policy ) {

    Ledger fork ( *this );

    bool result = block.apply ( fork, policy );

    if ( result ) {
    
        fork.setValue < u64 >( keyFor_blockHeightByHash ( block.getDigest ().toHex ()), block.getHeight ());
        fork.pushVersion ();
        
        this->takeSnapshot ( fork );
    }
    return result;
}

//----------------------------------------------------------------//
void Ledger::serializeEntitlements ( const Account& account, AbstractSerializerTo& serializer ) const {

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
void Ledger::setEntitlements ( string name, const Entitlements& entitlements ) {

    LedgerKey KEY_FOR_ENTITLEMENTS = keyFor_entitlements ( name );
    this->setObject < Entitlements >( KEY_FOR_ENTITLEMENTS, entitlements );
}

//----------------------------------------------------------------//
void Ledger::setEntropyString ( string entropy ) {

    this->setValue < string >( keyFor_entropy (), entropy );
}

//----------------------------------------------------------------//
void Ledger::setFeeDistributionPool ( u64 amount ) {

    this->setValue < u64 >( keyFor_feeDistributionPool (), amount );
}

//----------------------------------------------------------------//
LedgerResult Ledger::setFeeDistributionTable ( const FeeDistributionTable& distributionTable ) {

    if ( !distributionTable.isBalanced ()) return "Distribution table does not balance.";
    if ( !distributionTable.hasAccounts ( *this )) return "Distribution table names unknown accounts.";

    this->setObject < FeeDistributionTable >( keyFor_feeDistributionTable (), distributionTable );
    return true;
}

//----------------------------------------------------------------//
void Ledger::setFeeSchedule ( const FeeSchedule& feeSchedule ) {

    this->setObject < FeeSchedule >( keyFor_feeSchedule (), feeSchedule );
}

//----------------------------------------------------------------//
bool Ledger::setIdentity ( string identity ) {

    LedgerKey KEY_FOR_IDENTITY = keyFor_identity ();
    if ( this->hasValue ( KEY_FOR_IDENTITY )) return false;
    this->setValue < string >( KEY_FOR_IDENTITY, identity );
    return true;
}

//----------------------------------------------------------------//
void Ledger::setSchema ( const Schema& schema ) {

    string schemaHash = Digest ( schema, Digest::HASH_ALGORITHM_MD5 ).toHex ();

    this->setObject < Schema >( Ledger::keyFor_schema (), schema );
    this->setObject < SchemaVersion >( Ledger::keyFor_schemaVersion (), schema.mVersion );
    this->setValue < string >( Ledger::keyFor_schemaHash (), schemaHash );
    
    if ( !this->mSchemaCache ) {
        this->mSchemaCache = make_shared < map < string, Schema >>();
    }
    ( *this->mSchemaCache )[ schemaHash ] = schema;
}

//----------------------------------------------------------------//
void Ledger::setUnfinished ( const UnfinishedBlockList& unfinished ) {

    this->setObject < UnfinishedBlockList >( keyFor_unfinished (), unfinished );
}

//----------------------------------------------------------------//
void Ledger::setVOL ( u64 vol ) {

    LedgerFieldODBM < u64 > totalVOLField ( *this, Ledger::keyFor_VOL ());
    totalVOLField.set ( vol );
}

//----------------------------------------------------------------//
bool Ledger::verify ( const AssetMethodInvocation& invocation ) {

    const Schema& schema = this->getSchema ();
    const AssetMethod* method = schema.getMethodOrNull ( invocation.mMethodName );
    return ( method && ( method->mWeight == invocation.mWeight ) && ( method->mMaturity == invocation.mMaturity ));
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
Ledger& Ledger::AbstractLedgerComponent_getLedger () {

    return *this;
}

//----------------------------------------------------------------//
const Ledger& Ledger::AbstractLedgerComponent_getLedger () const {

    return *this;
}

//----------------------------------------------------------------//
void Ledger::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    this->init ();
    
    SerializableVector < Block > blocks;
    serializer.serialize ( "blocks", blocks );

    size_t size = blocks.size ();
    for ( size_t i = 0; i < size; ++i ) {
    
        Block block = blocks [ i ];
        this->pushBlock ( block, Block::VerificationPolicy::NONE );
    }
}

//----------------------------------------------------------------//
void Ledger::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

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
