// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Format.h>
#include <volition/Ledger.h>
#include <volition/SchemaLua.h>
#include <volition/TransactionMakerSignature.h>

namespace Volition {

//================================================================//
// Ledger
//================================================================//

//----------------------------------------------------------------//
bool Ledger::accountPolicy ( string accountName, const Policy* policy ) {

    return true;
}

//----------------------------------------------------------------//
bool Ledger::affirmKey ( string accountName, string keyName, const CryptoKey& key, string policyName ) {

    string keyID = key.getKeyID ();
    if ( keyID.size ()) return false;

    string keyInfoPrefix = KEY_ID + keyID;
    VersionedValue < KeyInfo > keyInfo ( *this, keyInfoPrefix );

    if (( keyInfo ) && ( keyInfo->mAccountName != accountName )) return false;

    VersionedValue < Account > account = this->getAccount ( accountName );
    if ( account ) {

        if ( key ) {
            
            Account updatedAccount = *account;
            updatedAccount.mKeys [ keyName ] = KeyAndPolicy ( key );
            this->setAccount ( accountName, updatedAccount );
            
            this->setValue < KeyInfo >( keyInfoPrefix, KeyInfo ( accountName, keyName ));
            
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------//
bool Ledger::awardAsset ( Schema& schema, string accountName, string assetName, int quantity ) {

    VersionedMap inventoryCollection ( *this, Ledger::getInventoryKey ( accountName ));

    BulkAssetIdentifier bulkIdentifier;

    if ( inventoryCollection.hasKey ( assetName )) {
        bulkIdentifier = inventoryCollection.getValue < BulkAssetIdentifier >( assetName );
        bulkIdentifier.mQuantity += quantity;
    }
    else {
        bulkIdentifier.mClassName = assetName;
        bulkIdentifier.mQuantity = quantity;
    }
    
    inventoryCollection.setValue < BulkAssetIdentifier >( assetName, bulkIdentifier );

    return true;
}

//----------------------------------------------------------------//
bool Ledger::checkMakerSignature ( const TransactionMakerSignature* makerSignature ) const {

    // TODO: actually check maker signature

    if ( makerSignature ) {
        VersionedValue < Account > account = this->getAccount ( makerSignature->getAccountName ());
        if ( account ) {
            return ( account->mNonce == makerSignature->getNonce ());
        }
    }
    return true;
}

//----------------------------------------------------------------//
bool Ledger::deleteKey ( string accountName, string keyName ) {

    AccountKey accountKey = this->getAccountKey ( accountName, keyName );
    if ( accountKey ) {
        Account updatedAccount = *accountKey.mAccount;
        updatedAccount.mKeys.erase ( keyName );
        this->setAccount ( accountName, updatedAccount );
        return true;
    }
    return false;
}

//----------------------------------------------------------------//
bool Ledger::genesisMiner ( string accountName, u64 amount, string keyName, const CryptoKey& key, string url ) {

    Account account;

    account.mBalance = amount;
    account.mKeys [ keyName ] = KeyAndPolicy ( key );
    this->setAccount ( accountName, account );

    string keyID = key.getKeyID ();
    assert ( keyID.size ());

    this->setValue < KeyInfo >( KEY_ID + keyID, KeyInfo ( accountName, keyName ));

    return this->registerMiner ( accountName, keyName, url );
}

//----------------------------------------------------------------//
VersionedValue < Account > Ledger::getAccount ( string accountName ) const {

    return VersionedValue < Account >( *this, ( prefixKey ( ACCOUNT, accountName )));
}

//----------------------------------------------------------------//
AccountKey Ledger::getAccountKey ( string accountName, string keyName ) const {

    AccountKey accountKey;
    accountKey.mKeyAndPolicy = NULL;

    accountKey.mAccount = this->getAccount ( accountName );
    if ( accountKey.mAccount ) {
        map < string, KeyAndPolicy >::const_iterator keyAndPolicyIt = accountKey.mAccount->mKeys.find ( keyName );
        if ( keyAndPolicyIt != accountKey.mAccount->mKeys.cend ()) {
            accountKey.mKeyAndPolicy = &keyAndPolicyIt->second;
        }
    }
    return accountKey;
}

//----------------------------------------------------------------//
VersionedValue < Block > Ledger::getBlock ( size_t height ) const {

    VersionedStore snapshot ( *this );
    if ( height < snapshot.getVersion ()) {
        snapshot.revert ( height );
    }
    return VersionedValue < Block >( snapshot, BLOCK_KEY );
}

//----------------------------------------------------------------//
Entropy Ledger::getEntropy () {

    VersionedValue < Entropy > entropy ( *this, ENTROPY );
    return entropy ? *entropy : Entropy ();
}

//----------------------------------------------------------------//
string Ledger::getInventoryKey ( string accountName ) {

    return Format::write ( INVENTORY_KEY_FMT_S, accountName.c_str ());
}

//----------------------------------------------------------------//
Inventory Ledger::getInventory ( string accountName ) const {

    Inventory inventory;
    
    try {
        VersionedCollectionIterator < BulkAssetIdentifier > collectionIt ( *this, Ledger::getInventoryKey ( accountName ));
        
        for ( ; collectionIt; ++collectionIt ) {
            inventory.mAssets.push_back ( *collectionIt );
        }
    }
    catch ( VersionedCollectionNotFoundException ) {
    }
    return inventory;
}

//----------------------------------------------------------------//
VersionedValue < KeyInfo > Ledger::getKeyInfo ( string keyID ) const {

    return VersionedValue < KeyInfo >( *this, KEY_ID + keyID );
}

//----------------------------------------------------------------//
VersionedValue < MinerInfo > Ledger::getMinerInfo ( string accountName ) const {

    return VersionedValue < MinerInfo >( *this, ( prefixKey ( MINER_INFO, accountName )));
}

//----------------------------------------------------------------//
map < string, MinerInfo > Ledger::getMiners () const {

    map < string, MinerInfo > minerInfoMap;

    const set < string >& miners = this->getValue < set < string >>( MINERS );
    set < string >::const_iterator minerIt = miners.cbegin ();
    for ( ; minerIt != miners.cend (); ++minerIt ) {
    
        const string& minerID = *minerIt;
        
        VersionedValue < MinerInfo > minerInfo = this->getMinerInfo ( minerID );
        assert ( minerInfo );
        minerInfoMap [ minerID ] = *minerInfo;
    }
    return minerInfoMap;
}

//----------------------------------------------------------------//
Ledger::MinerURLMap Ledger::getMinerURLs () const {

    return MinerURLMap ( *this, MINER_URLS );
}

//----------------------------------------------------------------//
string Ledger::getSchemaKey ( int schemaCount ) {

    return Format::write ( "%s%d", SCHEMA_PREFIX, schemaCount );
}

//----------------------------------------------------------------//
string Ledger::getSchemaNameKey ( string schemaName ) {

    return Format::write ( "%s%s", SCHEMA_PREFIX, schemaName.c_str ());
}

//----------------------------------------------------------------//
VersionedValue < Schema > Ledger::getSchema ( string schemaName ) const {

    return VersionedValue < Schema >( *this, Ledger::getSchemaNameKey ( schemaName ));
}

//----------------------------------------------------------------//
//list < Schema > Ledger::getSchemas () const {
//
//    list < Schema > schemaList;
//    const int schemaCount = this->getValue < int >( SCHEMA_COUNT );
//    for ( int i = 0; i < schemaCount; ++i ) {
//        const Schema& schema = this->getValue < Schema >( Ledger::getSchemaKey ( i ));
//        schemaList.push_back ( schema );
//    }
//    return schemaList;
//}

//----------------------------------------------------------------//
VersionedValue < Block > Ledger::getTopBlock () const {

    return VersionedValue < Block >( *this, BLOCK_KEY );
}

//----------------------------------------------------------------//
UnfinishedBlockList Ledger::getUnfinished () {

    VersionedValue < UnfinishedBlockList > unfinished ( *this, UNFINISHED );
    return unfinished ? *unfinished : UnfinishedBlockList ();
}

//----------------------------------------------------------------//
void Ledger::incrementNonce ( const TransactionMakerSignature* makerSignature ) {

    if ( makerSignature ) {

        u64 nonce = makerSignature->getNonce ();
        string accountName = makerSignature->getAccountName ();

        VersionedValue < Account > account = this->getAccount ( accountName );
        if ( account && ( account->mNonce <= nonce )) {
            Account updatedAccount = *account;
            updatedAccount.mNonce = nonce + 1;
            this->setAccount ( accountName, updatedAccount );
        }
    }
}

//----------------------------------------------------------------//
bool Ledger::keyPolicy ( string accountName, string policyName, const Policy* policy ) {

    return true;
}

//----------------------------------------------------------------//
Ledger::Ledger () {

    this->reset ();
}

//----------------------------------------------------------------//
Ledger::Ledger ( Ledger& other ) :
    VersionedStore ( other ) {
}

//----------------------------------------------------------------//
Ledger::~Ledger () {
}

//----------------------------------------------------------------//
bool Ledger::openAccount ( string accountName, string recipientName, u64 amount, string keyName, const CryptoKey& key ) {

    VersionedValue < Account > account = this->getAccount ( accountName );
    if ( account && ( account->mBalance >= amount )) {

        if ( this->getAccount ( recipientName )) return false;

        Account accountUpdated = *account;
        accountUpdated.mBalance -= amount;
        this->setAccount ( accountName, accountUpdated );
        
        Account recipient;
        recipient.mBalance = amount;
        recipient.mKeys [ keyName ] = KeyAndPolicy ( key );
        this->setAccount ( recipientName, recipient );

        return true;
    }
    return false;
}

//----------------------------------------------------------------//
string Ledger::prefixKey ( string prefix, string key ) {

    return prefix + "." + key;
}

//----------------------------------------------------------------//
bool Ledger::publishSchema ( string schemaName, const Schema& schema ) {

    schemaName = Ledger::getSchemaNameKey ( schemaName );

    if ( this->hasValue < Schema >( schemaName )) return false;

    int schemaCount = this->getValue < int >( SCHEMA_COUNT );
    
    string schemaKey = Ledger::getSchemaKey ( schemaCount );

    this->setValue < string >( schemaKey, schemaName );
    this->setValue < Schema >( schemaName, schema );
    this->setValue < int >( SCHEMA_COUNT, schemaCount + 1 );

    SchemaLua schemaLua ( schema );
    schemaLua.publish ( *this );
    
    return true;
}

//----------------------------------------------------------------//
bool Ledger::registerMiner ( string accountName, string keyName, string url ) {

    AccountKey accountKey = this->getAccountKey ( accountName, keyName );
    if ( accountKey ) {

        this->setValue < MinerInfo >( prefixKey ( MINER_INFO, accountName ), MinerInfo ( accountName, url, accountKey.mKeyAndPolicy->mKey ));
        
        // TODO: find an efficient way to do all this
        map < string, string > minerURLs = this->getValue < map < string, string >>( MINER_URLS );
        minerURLs [ accountName ] = url;
        this->setValue < map < string, string >>( MINER_URLS, minerURLs );

        set < string > miners = this->getValue < set < string >>( MINERS );
        miners.insert ( accountName );
        this->setValue < set < string >>( MINERS, miners );

        return true;
    }
    return false;
}

//----------------------------------------------------------------//
void Ledger::reset () {

    this->clear ();
    this->setValue < set < string >>( MINERS, set < string > ());
    this->setValue < map < string, string >>( MINER_URLS, map < string, string > ());
    this->setValue < int >( SCHEMA_COUNT, 0 );
}

//----------------------------------------------------------------//
bool Ledger::sendVOL ( string accountName, string recipientName, u64 amount ) {

    VersionedValue < Account > account      = this->getAccount ( accountName );
    VersionedValue < Account > recipient    = this->getAccount ( recipientName );

    if ( account && recipient && ( account->mBalance >= amount )) {
    
        Account accountUpdated = *account;
        Account recipientUpdated = *recipient;
    
        accountUpdated.mBalance -= amount;
        recipientUpdated.mBalance += amount;
        
        this->setAccount ( accountName, accountUpdated );
        this->setAccount ( recipientName, recipientUpdated );
        
        return true;
    }
    return false;
}

//----------------------------------------------------------------//
void Ledger::setAccount ( string accountName, const Account& account ) {

    this->setValue < Account >( prefixKey ( ACCOUNT, accountName ), account );
}

//----------------------------------------------------------------//
void Ledger::setBlock ( const Block& block ) {
    assert ( block.mHeight == this->getVersion ());
    this->setValue < Block >( BLOCK_KEY, block );
}

//----------------------------------------------------------------//
void Ledger::setEntropy ( const Entropy& entropy ) {

    this->setValue < Entropy >( ENTROPY, entropy );
}

//----------------------------------------------------------------//
void Ledger::setMinerInfo ( string accountName, const MinerInfo& minerInfo ) {

    this->setValue < MinerInfo >( prefixKey ( ACCOUNT, accountName ), minerInfo );
}

//----------------------------------------------------------------//
void Ledger::setUnfinished ( const UnfinishedBlockList& unfinished ) {

    this->setValue < UnfinishedBlockList >( UNFINISHED, unfinished );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
