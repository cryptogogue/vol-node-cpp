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
#include <volition/TransactionMaker.h>

namespace Volition {

//================================================================//
// Ledger
//================================================================//

//----------------------------------------------------------------//
bool Ledger::affirmKey ( string accountName, string makerKeyName, string keyName, const CryptoKey& key, const Policy* policy ) {

    string keyID = key.getKeyID ();
    if ( keyID.size ()) return false;

    shared_ptr < Account > account = this->getAccount ( accountName );
    if ( account ) {

        const LedgerKey KEY_FOR_ACCOUNT_KEY_LOOKUP = keyFor_accountKeyLookup ( keyID );
        shared_ptr < AccountKeyLookup > accountKeyLookup = this->getObjectOrNull < AccountKeyLookup >( KEY_FOR_ACCOUNT_KEY_LOOKUP );

        // keys must be unique to accounts; no sharing keys across multiple accounts!
        if ( accountKeyLookup && ( accountKeyLookup->mAccountIndex != account->mIndex )) return false;

        if ( key ) {
            
            const KeyAndPolicy* makerKeyAndPolicy = account->getKeyAndPolicyOrNull ( makerKeyName );
            if ( !makerKeyAndPolicy ) return false;
            
            const Policy* selectedPolicy = policy;
            if ( selectedPolicy ) {
                if ( !this->isValidPolicy < KeyEntitlements >( *selectedPolicy, makerKeyAndPolicy->mPolicy )) return false;
            }
            else {
                const KeyAndPolicy* keyAndPolicy = account->getKeyAndPolicyOrNull ( makerKeyName );
                selectedPolicy = keyAndPolicy ? &keyAndPolicy->mPolicy : &makerKeyAndPolicy->mPolicy;
            }
            
            account->mKeys [ keyName ] = KeyAndPolicy ( key, *selectedPolicy );
            this->setAccount ( *account );
            
            this->setObject < AccountKeyLookup >( KEY_FOR_ACCOUNT_KEY_LOOKUP, AccountKeyLookup ( account->mIndex, keyName ));
            
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------//
bool Ledger::awardAsset ( const Schema& schema, string accountName, string assetType, size_t quantity ) {

    if ( quantity == 0 ) return true;

    if ( !schema.getDefinitionOrNull ( assetType )) return false;

    AccountODBM accountODBM ( *this, this->getAccountIndex ( accountName ));
    if ( accountODBM.mIndex == Account::NULL_INDEX ) return false;

    LedgerKey KEY_FOR_GLOBAL_ASSET_COUNT = keyFor_globalAssetCount ();
    size_t globalAssetCount = this->getValueOrFallback < size_t >( KEY_FOR_GLOBAL_ASSET_COUNT, 0 );
    size_t accountAssetCount = accountODBM.mAssetCount.get ( 0 );
    
    for ( size_t i = 0; i < quantity; ++i ) {
        
        AssetODBM assetODBM ( *this, globalAssetCount + i );
                
        assetODBM.mOwner.set ( accountODBM.mIndex );
        assetODBM.mPosition.set ( accountAssetCount + i );
        assetODBM.mType.set ( assetType );
        
        accountODBM.getInventoryField ( assetODBM.mPosition.get ()).set ( assetODBM.mIndex );
    }
    
    this->setValue < size_t >( KEY_FOR_GLOBAL_ASSET_COUNT, globalAssetCount + quantity );
    accountODBM.mAssetCount.set ( accountAssetCount + quantity );

    return true;
}

//----------------------------------------------------------------//
bool Ledger::deleteKey ( string accountName, string keyName ) {

    AccountKey accountKey = this->getAccountKey ( accountName, keyName );
    if ( accountKey ) {
        Account updatedAccount = *accountKey.mAccount;
        updatedAccount.mKeys.erase ( keyName );
        this->setAccount ( updatedAccount );
        return true;
    }
    return false;
}

//----------------------------------------------------------------//
shared_ptr < Account > Ledger::getAccount ( Account::Index index ) const {

    return this->getObjectOrNull < Account >( AccountODBM::keyFor_body ( index ));
}

//----------------------------------------------------------------//
shared_ptr < Account > Ledger::getAccount ( string accountName ) const {

    Account::Index accountIndex = this->getAccountIndex ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return NULL;
    return this->getAccount ( accountIndex );
}

//----------------------------------------------------------------//
Account::Index Ledger::getAccountIndex ( string accountName ) const {

    if ( accountName.size () == 0 ) return Account::NULL_INDEX;

    string lowerName = Format::tolower ( accountName );
    LedgerKey KEY_FOR_ACCOUNT_ALIAS = keyFor_accountAlias ( lowerName );
    return this->getValueOrFallback < Account::Index >( KEY_FOR_ACCOUNT_ALIAS, Account::NULL_INDEX );
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
shared_ptr < AccountKeyLookup > Ledger::getAccountKeyLookup ( string keyID ) const {

    return this->getObjectOrNull < AccountKeyLookup >( keyFor_accountKeyLookup ( keyID ));
}

//----------------------------------------------------------------//
string Ledger::getAccountName ( Account::Index accountIndex ) const {

    return this->getValueOrFallback < string >( AccountODBM::keyFor_name ( accountIndex ), "" );
}

//----------------------------------------------------------------//
u64 Ledger::getAccountNonce ( Account::Index accountIndex ) const {

    return this->getValueOrFallback < u64 >( AccountODBM::keyFor_nonce ( accountIndex ), 0 );
}

//----------------------------------------------------------------//
shared_ptr < Asset > Ledger::getAsset ( const Schema& schema, AssetID::Index index ) const {

    AssetODBM assetODBM ( *this, index );
    if ( !assetODBM.mOwner.exists ()) return NULL;

    const AssetDefinition* assetDefinition = schema.getDefinitionOrNull ( assetODBM.mType.get ());
    if ( !assetDefinition ) return NULL;
    
    shared_ptr < Asset > asset = make_shared < Asset >();
    asset->mType    = assetODBM.mType.get ();
    asset->mAssetID = assetODBM.mIndex;
    asset->mOwner   = this->getAccountName ( assetODBM.mOwner.get ());
    
    // copy the fields and apply any overrides
    AssetDefinition::Fields::const_iterator fieldIt = assetDefinition->mFields.cbegin ();
    for ( ; fieldIt != assetDefinition->mFields.cend (); ++fieldIt ) {
        
        string fieldName = fieldIt->first;
        
        const AssetFieldDefinition& field = fieldIt->second;
        AssetFieldValue value = field;
        
//        if ( field.mMutable ) {
//
//            string keyforAssetModifiedField = this->formatKeyforAssetModifiedField ( identifier, fieldName );
//
//            switch ( field.mType ) {
//                case AssetTemplateField::Type::NUMERIC:
//                    value = this->getValueOrFallback < double >( keyforAssetModifiedField, value.mNumeric );
//                    break;
//                case AssetTemplateField::Type::STRING:
//                    value = this->getValueOrFallback < string >( keyforAssetModifiedField, value.mString );
//                    break;
//            }
//            asset->mFields [ fieldName ] = value;
//        }
        asset->mFields [ fieldName ] = value;
    }
    return asset;
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
string Ledger::getIdentity () const {

    return this->getValueOrFallback < string >( keyFor_identity (), "" );
}

//----------------------------------------------------------------//
SerializableList < Asset > Ledger::getInventory ( const Schema& schema, string accountName, size_t max ) const {

    SerializableList < Asset > assets;

    AccountODBM accountODBM ( *this, this->getAccountIndex ( accountName ));
    if ( accountODBM.mIndex == Account::NULL_INDEX ) return assets;

    size_t assetCount = accountODBM.mAssetCount.get ();
    
    if (( max > 0 ) && ( max < assetCount )) {
        assetCount = max;
    }
    
    for ( size_t i = 0; i < assetCount; ++i ) {
    
        AssetID::Index assetIndex = accountODBM.getInventoryField ( i ).get ();
        shared_ptr < Asset > asset = this->getAsset ( schema, assetIndex );
        assert ( asset );
        assets.push_back ( *asset );
    }
    return assets;
}

//----------------------------------------------------------------//
shared_ptr < MinerInfo > Ledger::getMinerInfo ( string accountName ) const {

    Account::Index accountIndex = this->getAccountIndex ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return NULL;

    return this->getObjectOrNull < MinerInfo >( AccountODBM::keyFor_minerInfo ( accountIndex ));
}

//----------------------------------------------------------------//
map < string, MinerInfo > Ledger::getMiners () const {

    map < string, MinerInfo > minerInfoMap;

    shared_ptr < SerializableSet < string >> miners = this->getObjectOrNull < SerializableSet < string >>( keyFor_miners ());
    assert ( miners );
    
    set < string >::const_iterator minerIt = miners->cbegin ();
    for ( ; minerIt != miners->cend (); ++minerIt ) {
    
        const string& minerID = *minerIt;
        
        shared_ptr < MinerInfo > minerInfo = this->getMinerInfo ( minerID );
        assert ( minerInfo );
        minerInfoMap [ minerID ] = *minerInfo;
    }
    return minerInfoMap;
}

//----------------------------------------------------------------//
shared_ptr < Ledger::MinerURLMap > Ledger::getMinerURLs () const {

    return this->getObjectOrNull < MinerURLMap >( keyFor_minerURLs ());
}

//----------------------------------------------------------------//
void Ledger::getSchema ( Schema& schema ) const {

    string schemaString = this->getSchemaString ();
    if ( schemaString.size () > 0 ) {
        FromJSONSerializer::fromJSONString ( schema, schemaString );
    }
}

//----------------------------------------------------------------//
string Ledger::getSchemaString () const {

    return this->getValue < string >( keyFor_schema ());
}

//----------------------------------------------------------------//
string Ledger::getTransactionNote ( string accountName, u64 nonce ) const {

    Account::Index accountIndex = this->getAccountIndex ( accountName );
    if ( accountIndex != Account::NULL_INDEX ) {

        LedgerKey KEY_FOR_ACCOUNT_TRANSACTION_NOTE = AccountODBM::keyFor_transactionNoteField ( accountIndex, nonce );
        return this->getValueOrFallback < string >( KEY_FOR_ACCOUNT_TRANSACTION_NOTE, "" );
    }
    return "";
}

//----------------------------------------------------------------//
UnfinishedBlockList Ledger::getUnfinished () {

    shared_ptr < UnfinishedBlockList > unfinished = this->getObjectOrNull < UnfinishedBlockList >( keyFor_unfinished ());
    return unfinished ? *unfinished : UnfinishedBlockList ();
}

//----------------------------------------------------------------//
void Ledger::incrementNonce ( Account::Index index, u64 nonce, string note ) {

    AccountODBM accountODBM ( *this, index );
    if ( !accountODBM.mBody.exists ()) return;
    if ( accountODBM.mNonce.get ( 0 ) != nonce ) return;
    
    LedgerKey KEY_FOR_ACCOUNT_TRANSACTION_NOTE = AccountODBM::keyFor_transactionNoteField ( index, nonce );
    this->setValue < string >( KEY_FOR_ACCOUNT_TRANSACTION_NOTE, note );

    accountODBM.mNonce.set ( nonce + 1 );
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
bool Ledger::invoke ( const Schema& schema, string accountName, const AssetMethodInvocation& invocation ) {

    const AssetMethod* method = schema.getMethodOrNull ( invocation.mMethodName );
    if ( !( method && ( method->mWeight == invocation.mWeight ) && ( method->mMaturity == invocation.mMaturity ))) return false;

    // make sure account exists
    Account::Index accountIndex = this->getAccountIndex ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return false;

    // TODO: this is brutally inefficient, but we're doing it for now. can add a cache of LuaContext objects later to speed things up.
    LuaContext lua ( *this, schema, method->mLua );
    return lua.invoke ( accountName, *method, invocation );
}

//----------------------------------------------------------------//
bool Ledger::isAccountName ( string accountName ) {

    size_t size = accountName.size ();
    for ( size_t i = 0; i < size; ++i ) {
        const char c = accountName [ i ];
        if ( !isgraph ( c )) return false;
    }
    return true;
}

//----------------------------------------------------------------//
bool Ledger::isChildName ( string accountName ) {

    return ( accountName [ 0 ] == '~' );
}

//----------------------------------------------------------------//
bool Ledger::isSuffix ( string suffix ) {

    // child names must follow the format "<hex3>.<hex3>.<hex3>", where each hex3 is a *lowecase* 3-digit hexadecimal number.
    
    if ( suffix.size () != 11 ) return false;
    if (( suffix [ 3 ] != '.' ) || ( suffix [ 7 ] != '.' )) return false;
    
    for ( size_t i = 0; i < 11; ++i ) {
    
        if (( i == 3 ) || ( i == 7 )) continue;
    
        char c = suffix [ i ];
    
        if (( '0' <= c ) || ( c <= '9' )) continue;
        if (( 'a' <= c ) || ( c <= 'f' )) continue;
        
        return false;
    }
    return true;
}

//----------------------------------------------------------------//
bool Ledger::isGenesis () const {

    return ( this->getVersion () == 0 );
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
bool Ledger::newAccount ( string accountName, u64 balance, string keyName, const CryptoKey& key, const Policy& keyPolicy, const Policy& accountPolicy ) {

    // check to see if there is already an alias for this account name
    string lowerName = Format::tolower ( accountName );
    LedgerKey KEY_FOR_ACCOUNT_ALIAS = keyFor_accountAlias ( lowerName );
    if ( this->hasKey ( KEY_FOR_ACCOUNT_ALIAS )) return false; // alias already exists

    // provision the account ID
    LedgerKey KEY_FOR_GLOBAL_ACCOUNT_COUNT = keyFor_globalAccountCount ();
    Account::Index accountIndex = this->getValue < Account::Index >( KEY_FOR_GLOBAL_ACCOUNT_COUNT );
    this->setValue < Account::Index >( KEY_FOR_GLOBAL_ACCOUNT_COUNT, accountIndex + 1 ); // increment counter

    // store the account
    Account account;
    account.mPolicy = accountPolicy;
    account.mIndex = accountIndex;
    account.mName = accountName;
    account.mBalance = balance;
    account.mKeys [ MASTER_KEY_NAME ] = KeyAndPolicy ( key, keyPolicy );
    
    this->setObject < Account >( AccountODBM::keyFor_body ( accountIndex ), account );

    // store the key (for reverse lookup):
    string keyID = key.getKeyID ();
    assert ( keyID.size ());
    this->setObject < AccountKeyLookup >( keyFor_accountKeyLookup ( keyID ), AccountKeyLookup ( accountIndex, keyName ));

    // store the alias
    this->setValue < Account::Index >( KEY_FOR_ACCOUNT_ALIAS, accountIndex );
    this->setValue < string >( AccountODBM::keyFor_name ( accountIndex ), accountName );

    return true;
}

//----------------------------------------------------------------//
bool Ledger::registerMiner ( string accountName, string keyName, string url ) {

    AccountKey accountKey = this->getAccountKey ( accountName, keyName );
    if ( accountKey ) {

        shared_ptr < Account > account = accountKey.mAccount;
        Account::Index accountIndex = account->mIndex;

        this->setObject < MinerInfo >(
            AccountODBM::keyFor_minerInfo ( accountIndex ),
            MinerInfo ( accountIndex, url, accountKey.mKeyAndPolicy->mKey )
        );
        
        // TODO: find an efficient way to do all this
        
        LedgerKey KEY_FOR_MINERS = keyFor_miners ();
        shared_ptr < SerializableSet < string >> miners = this->getObjectOrNull < SerializableSet < string >>( KEY_FOR_MINERS );
        assert ( miners );
        miners->insert ( accountName );
        this->setObject < SerializableSet < string >>( KEY_FOR_MINERS, *miners );
        
        LedgerKey KEY_FOR_MINER_URLS = keyFor_minerURLs ();
        shared_ptr < SerializableMap < string, string >> minerURLs = this->getObjectOrNull < SerializableMap < string, string >>( KEY_FOR_MINER_URLS );
        assert ( minerURLs );
        ( *minerURLs )[ accountName ] = url;
        this->setObject < SerializableMap < string, string >>( KEY_FOR_MINER_URLS, *minerURLs );

        return true;
    }
    return false;
}

//----------------------------------------------------------------//
void Ledger::setAccount ( const Account& account ) {

    assert ( account.mIndex != Account::NULL_INDEX );
    this->setObject < Account >( AccountODBM::keyFor_body ( account.mIndex ), account );
}

//----------------------------------------------------------------//
bool Ledger::setAssetFieldValue ( const Schema& schema, AssetID::Index index, string fieldName, const AssetFieldValue& field ) {

    // make sure the asset exists
    LedgerKey KEY_FOR_ASSET_TYPE = AssetODBM::keyFor_type ( index );
    if ( !this->hasValue ( KEY_FOR_ASSET_TYPE )) return false;
    string assetType = this->getValue < string >( KEY_FOR_ASSET_TYPE );

    // make sure the field exists
    const AssetDefinition* assetDefinition = schema.getDefinitionOrNull ( assetType );
    if ( !assetDefinition ) return false;
    if ( !assetDefinition->hasMutableField ( fieldName, field.getType ())) return false;

//    // set the field
//    string keyforAssetModifiedField = Ledger::formatKeyforAssetModifiedField ( identifier, fieldName );
//
//    switch ( field.mType ) {
//        case AssetTemplateField::Type::NUMERIC:
//            this->setValue < double >( keyforAssetModifiedField, field.mValue.mNumeric );
//            break;
//        case AssetTemplateField::Type::STRING:
//            this->setValue < string >( keyforAssetModifiedField, field.mValue.mString );
//            break;
//    }

    return true;
}

//----------------------------------------------------------------//
void Ledger::setBlock ( const Block& block ) {
    assert ( block.mHeight == this->getVersion ());
    this->setObject < Block >( keyFor_block (), block );
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

} // namespace Volition
