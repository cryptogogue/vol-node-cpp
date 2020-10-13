// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AccountODBM.h>
#include <volition/Asset.h>
#include <volition/AssetMethod.h>
#include <volition/AssetMethodInvocation.h>
#include <volition/AssetODBM.h>
#include <volition/Block.h>
#include <volition/Format.h>
#include <volition/Ledger.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/LuaContext.h>
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
        shared_ptr < Block > block = this->getBlock ( blockID );
        if ( !block ) return false;
        if (( blockID > 0 ) && ( block->getMinerID () != minerID )) return false;
        blockID++;
    }
    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger::checkSchemaMethods ( const Schema& schema ) const {

    string out;

    LuaContext lua ( *this, schema, 0 );

    Schema::Methods::const_iterator methodIt = schema.mMethods.cbegin ();
    for ( ; methodIt != schema.mMethods.cend (); ++methodIt ) {
        const AssetMethod& method = methodIt->second;
        LedgerResult result = lua.compile ( method );
        if ( !result ) {
        
            Format::write ( out, "LUA COMPILATION ERROR IN %s:\n", methodIt->first.c_str ());
            out.append ( result.getMessage ());
            out.append ( "\n" );
        }
    }
    
    return out;
}

//----------------------------------------------------------------//
void Ledger::clearSchemaCache () {

    this->mSchemaCache.clear ();
}

//----------------------------------------------------------------//
size_t Ledger::countBlocks () const {

    return this->getVersion ();
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
shared_ptr < Block > Ledger::getBlock () const {

    return this->getObjectOrNull < Block >( keyFor_block ());
}

//----------------------------------------------------------------//
shared_ptr < Block > Ledger::getBlock ( size_t height ) const {

    VersionedStore snapshot ( *this );
    if ( height < snapshot.getVersion ()) {
        snapshot.revert ( height );
    }
    return Ledger::getObjectOrNull < Block >( snapshot, keyFor_block ());
}

//----------------------------------------------------------------//
string Ledger::getBlockHash () const {

    return this->getValue < string >( keyFor_blockHash ());
}

//----------------------------------------------------------------//
u64 Ledger::getBlockSize () const {

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
string Ledger::getGenesisHash () const {

    return this->getValueOrFallback < string >( keyFor_genesisHash (), "" );
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
const Schema& Ledger::getSchema () {

    string schemaHash = this->getSchemaHash ();
    
    map < string, Schema >::const_iterator schemaIt = this->mSchemaCache.find ( schemaHash );
    if ( schemaIt != this->mSchemaCache.cend ()) {
        return schemaIt->second;
    }
    
    Schema& schema = this->mSchemaCache [ schemaHash ];
    this->getSchema ( schema );
    return schema;
}

//----------------------------------------------------------------//
void Ledger::getSchema ( Schema& schema ) const {

    string schemaString = this->getSchemaString ();
    if ( schemaString.size () > 0 ) {
        FromJSONSerializer::fromJSONString ( schema, schemaString );
    }
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

    Account::Index accountIndex = this->getAccountIndex ( accountName );
    if ( accountIndex != Account::NULL_INDEX ) {
    
        return AccountODBM ( this->getLedger (), accountIndex ).getTransactionLookupField ( uuid ).exists ();
    }
    return false;
}

//----------------------------------------------------------------//
void Ledger::init () {

    this->clear ();
    this->setObject < SerializableSet < string >>( keyFor_miners (), SerializableSet < string > ());
    this->setObject < SerializableMap < string, string >>( keyFor_minerURLs (), SerializableMap < string, string > ());
    this->setValue < AssetID::Index >( keyFor_globalAccountCount (), 0 );
    this->setValue < AssetID::Index >( keyFor_globalAssetCount (), 0 );
    this->setValue < string >( keyFor_schema (), "{}" );
    this->setValue < u64 >( keyFor_blockSize (), DEFAULT_BLOCK_SIZE );
}

//----------------------------------------------------------------//
LedgerResult Ledger::invoke ( const Schema& schema, string accountName, const AssetMethodInvocation& invocation, time_t time ) {

    const AssetMethod* method = schema.getMethodOrNull ( invocation.mMethodName );
    if ( !( method && ( method->mWeight == invocation.mWeight ) && ( method->mMaturity == invocation.mMaturity ))) return false;

    // make sure account exists
    Account::Index accountIndex = this->getAccountIndex ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return false;

    // TODO: this is brutally inefficient, but we're doing it for now. can add a cache of LuaContext objects later to speed things up.
    LuaContext lua ( *this, schema, time );
    return lua.invoke ( accountName, *method, invocation );
}

//----------------------------------------------------------------//
bool Ledger::isGenesis () const {

    return ( this->getHeight () == 0 );
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
            shared_ptr < Block > block = this->getBlock ( i );
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
void Ledger::setBlock ( const Block& block ) {

    assert ( block.mHeight == this->getVersion ());

    string hash = block.getDigest ();

    this->setObject < Block >( keyFor_block (), block );
    this->setValue < string >( keyFor_blockHash (), hash );
    this->setValue < string >( keyFor_blockPose (), block.getPose ());

    if ( block.mHeight == 0 ) {
        this->setValue < string >( keyFor_genesisHash (), hash );
    }
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
}

//----------------------------------------------------------------//
void Ledger::setUnfinished ( const UnfinishedBlockList& unfinished ) {

    this->setObject < UnfinishedBlockList >( keyFor_unfinished (), unfinished );
}

//----------------------------------------------------------------//
bool Ledger::verify ( const Schema& schema, const AssetMethodInvocation& invocation ) const {

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
    
    size_t top = this->getVersion ();
    VersionedStoreIterator chainIt ( *this, 0 );
    for ( ; chainIt && ( chainIt.getVersion () < top ); chainIt.next ()) {
        shared_ptr < Block > block = Ledger::getObjectOrNull < Block >( chainIt, keyFor_block ());
        assert ( block );
        blocks.push_back ( *block );
    }
    serializer.serialize ( "blocks", blocks );
}

} // namespace Volition
