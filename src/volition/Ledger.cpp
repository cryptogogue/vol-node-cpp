// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Asset.h>
#include <volition/AssetMethod.h>
#include <volition/AssetMethodInvocation.h>
#include <volition/Block.h>
#include <volition/Format.h>
#include <volition/Ledger.h>
#include <volition/LuaContext.h>
#include <volition/TransactionMaker.h>

namespace Volition {

//================================================================//
// Ledger
//================================================================//

//----------------------------------------------------------------//
bool Ledger::accountPolicy ( string accountName, const Policy* policy ) {
    UNUSED ( policy );
    UNUSED ( accountName );

    return true;
}

//----------------------------------------------------------------//
bool Ledger::affirmKey ( string accountName, string keyName, const CryptoKey& key, string policyName ) {
    UNUSED ( policyName );

    string keyID = key.getKeyID ();
    if ( keyID.size ()) return false;

    shared_ptr < Account > account = this->getAccount ( accountName );
    if ( account ) {

        const LedgerKey KEY_FOR_ACCOUNT_KEY_LOOKUP = FormatLedgerKey::forAccountKeyLookup ( keyID );
        shared_ptr < AccountKeyLookup > accountKeyLookup = this->getObjectOrNull < AccountKeyLookup >( KEY_FOR_ACCOUNT_KEY_LOOKUP );

        if ( accountKeyLookup && ( accountKeyLookup->mAccountIndex != account->mIndex )) return false;

        if ( key ) {
            
            account->mKeys [ keyName ] = KeyAndPolicy ( key );
            this->setAccount ( accountName, *account );
            
            this->setObject < AccountKeyLookup >( KEY_FOR_ACCOUNT_KEY_LOOKUP, AccountKeyLookup ( account->mIndex, keyName ));
            
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------//
bool Ledger::awardAsset ( string accountName, string assetType, int quantity ) {

    if ( quantity == 0 ) return true;

    LedgerKey KEY_FOR_ASSET_DEFINITION = FormatLedgerKey::forAssetDefinition ( assetType );
    if ( !this->hasValue ( KEY_FOR_ASSET_DEFINITION )) return false;

    Account::Index accountIndex = this->getAccountIndex ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return false;

    LedgerKey KEY_FOR_ASSET_COUNT = FormatLedgerKey::forAssetCount ();
    Asset::Index firstAssetIndex = this->getValue < Asset::Index >( KEY_FOR_ASSET_COUNT );

    // update head
    LedgerKey KEY_FOR_ACCOUNT_HEAD = FormatLedgerKey::forAccountMember ( accountIndex, FormatLedgerKey::ACCOUNT_HEAD );
    Asset::Index accountHeadIndex = this->getValueOrFallback < Asset::Index >( KEY_FOR_ACCOUNT_HEAD, Asset::NULL_INDEX );

    if ( accountHeadIndex == Asset::NULL_INDEX ) {
        this->setValue < Asset::Index >( KEY_FOR_ACCOUNT_HEAD, firstAssetIndex );
    }

    // update tail
    string KEY_FOR_ACCOUNT_TAIL = FormatLedgerKey::forAccountMember ( accountIndex, FormatLedgerKey::ACCOUNT_TAIL );
    Asset::Index accountTailIndex = this->getValueOrFallback < Asset::Index >( KEY_FOR_ACCOUNT_TAIL, Asset::NULL_INDEX );
    
    if ( accountTailIndex != Asset::NULL_INDEX ) {
        this->setValue < Asset::Index >( FormatLedgerKey::forAssetMember ( accountTailIndex, FormatLedgerKey::ASSET_NEXT ), firstAssetIndex );
    }
    Asset::Index firstElementPrev = accountTailIndex;
    accountTailIndex = ( firstAssetIndex + quantity ) - 1;
    this->setValue < Asset::Index >( KEY_FOR_ACCOUNT_TAIL, accountTailIndex );
    
    // now add the assets
    for ( Asset::Index i = firstAssetIndex; i <= accountTailIndex; ++i ) {
        
        this->setValue < string >( FormatLedgerKey::forAssetMember ( i, FormatLedgerKey::ASSET_OWNER ), accountName );
        this->setValue < string >( FormatLedgerKey::forAssetMember ( i, FormatLedgerKey::ASSET_TYPE ), assetType );
        
        string KEY_FOR_ASSET_PREV = FormatLedgerKey::forAssetMember ( i, FormatLedgerKey::ASSET_PREV );
        string KEY_FOR_ASSET_NEXT = FormatLedgerKey::forAssetMember ( i, FormatLedgerKey::ASSET_NEXT );
        
        if ( i == firstAssetIndex ) {
            this->setValue < Asset::Index >( KEY_FOR_ASSET_PREV, firstElementPrev );
        }
        else {
            this->setValue < Asset::Index >( KEY_FOR_ASSET_PREV, i - 1 );
        }
        
        if ( i < accountTailIndex ) {
            this->setValue < Asset::Index >( KEY_FOR_ASSET_NEXT, i + 1 );
        }
        else {
            this->setValue < Asset::Index >( KEY_FOR_ASSET_NEXT, Asset::NULL_INDEX );
        }
    }
    
    // increment total asset count
    this->setValue < size_t >( KEY_FOR_ASSET_COUNT, accountTailIndex + 1 );

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
bool Ledger::genesisMiner ( string accountName, u64 balance, const CryptoKey& key, string url ) {

    if ( !this->newAccount ( accountName, balance, key )) return false;
    return this->registerMiner ( accountName, MASTER_KEY_NAME, url );
}

//----------------------------------------------------------------//
shared_ptr < Account > Ledger::getAccount ( string accountName ) const {

    Account::Index accountIndex = this->getAccountIndex ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return NULL;

    return this->getObjectOrNull < Account >( FormatLedgerKey::forAccount ( accountIndex ));
}

//----------------------------------------------------------------//
Account::Index Ledger::getAccountIndex ( string accountName ) const {

    if ( accountName.size () == 0 ) return Account::NULL_INDEX;

    LedgerKey KEY_FOR_ACCOUNT_ALIAS = FormatLedgerKey::forAccountAlias ( accountName );
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

    return this->getObjectOrNull < AccountKeyLookup >( FormatLedgerKey::forAccountKeyLookup ( keyID ));
}

//----------------------------------------------------------------//
string Ledger::getAccountName ( Account::Index accountIndex ) const {

    return this->getValueOrFallback < string >( FormatLedgerKey::forAccountMember ( accountIndex, FormatLedgerKey::ACCOUNT_NAME ), "" );
}

//----------------------------------------------------------------//
shared_ptr < Asset > Ledger::getAsset ( Asset::Index index ) const {

    string KEY_FOR_ASSET_OWNER = FormatLedgerKey::forAssetMember ( index, FormatLedgerKey::ASSET_OWNER );
    if ( !this->hasValue ( KEY_FOR_ASSET_OWNER )) return NULL;

    LedgerKey KEY_FOR_ASSET_TYPE = FormatLedgerKey::forAssetMember ( index, FormatLedgerKey::ASSET_TYPE );
    string typeName = this->getValue < string >( KEY_FOR_ASSET_TYPE );

    LedgerKey KEY_FOR_ASSET_DEFINITION = FormatLedgerKey::forAssetDefinition ( typeName );
    shared_ptr < AssetDefinition > assetDefinition = this->getObjectOrNull < AssetDefinition >( KEY_FOR_ASSET_DEFINITION );
    if ( !assetDefinition ) return NULL;
    
    shared_ptr < Asset > asset = make_shared < Asset >( assetDefinition );
    asset->mType    = typeName;
    asset->mIndex   = index;
    asset->mOwner   = this->getValue < string >( KEY_FOR_ASSET_OWNER );
    
    // copy the fields and apply any overrides
    AssetDefinition::Fields::const_iterator fieldIt = assetDefinition->mFields.cbegin ();
    for ( ; fieldIt != assetDefinition->mFields.cend (); ++fieldIt ) {
        
        string fieldName = fieldIt->first;
        
        const AssetDefinitionField& field = fieldIt->second;
        AssetFieldValue value = field;
        
//        if ( field.mMutable ) {
//
//            string keyForAssetField = this->formatKeyForAssetField ( identifier, fieldName );
//
//            switch ( field.mType ) {
//                case AssetTemplateField::Type::NUMERIC:
//                    value = this->getValueOrFallback < double >( keyForAssetField, value.mNumeric );
//                    break;
//                case AssetTemplateField::Type::STRING:
//                    value = this->getValueOrFallback < string >( keyForAssetField, value.mString );
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

    return this->getObjectOrNull < Block >( FormatLedgerKey::forBlock ());
}

//----------------------------------------------------------------//
shared_ptr < Block > Ledger::getBlock ( size_t height ) const {

    VersionedStore snapshot ( *this );
    if ( height < snapshot.getVersion ()) {
        snapshot.revert ( height );
    }
    return Ledger::getObjectOrNull < Block >( snapshot, FormatLedgerKey::forBlock ());
}

//----------------------------------------------------------------//
Entropy Ledger::getEntropy () const {

    string entropy = this->getValueOrFallback < string >( FormatLedgerKey::forEntropy (), "" );
    return entropy.size () > 0 ? Entropy ( entropy ) : Entropy ();
}

//----------------------------------------------------------------//
string Ledger::getIdentity () const {

    return this->getValueOrFallback < string >( FormatLedgerKey::forIdentity (), "" );
}

//----------------------------------------------------------------//
SerializableList < Asset > Ledger::getInventory ( string accountName ) const {

    SerializableList < Asset > assets;

    Account::Index accountIndex = this->getAccountIndex ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return assets;

    LedgerKey KEY_FOR_ACCOUNT_HEAD = FormatLedgerKey::forAccountMember ( accountIndex, FormatLedgerKey::ACCOUNT_HEAD );
    Asset::Index cursor = this->getValueOrFallback < Asset::Index >( KEY_FOR_ACCOUNT_HEAD, Asset::NULL_INDEX );
    
    while ( cursor != Asset::NULL_INDEX ) {
    
        shared_ptr < Asset > asset = this->getAsset ( cursor );
        assert ( asset );
        assets.push_back ( *asset );
    
        cursor = this->getValue < Asset::Index >( FormatLedgerKey::forAssetMember ( cursor, FormatLedgerKey::ASSET_NEXT ));
    }
    return assets;
}

//----------------------------------------------------------------//
shared_ptr < AssetMethod > Ledger::getMethod ( string methodName ) const {

    return this->getObjectOrNull < AssetMethod >( FormatLedgerKey::forMethod ( methodName ));
}

//----------------------------------------------------------------//
shared_ptr < MinerInfo > Ledger::getMinerInfo ( string accountName ) const {

    Account::Index accountIndex = this->getAccountIndex ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return NULL;

    return this->getObjectOrNull < MinerInfo >( FormatLedgerKey::forAccountMember ( accountIndex, FormatLedgerKey::ACCOUNT_MINER_INFO ));
}

//----------------------------------------------------------------//
map < string, MinerInfo > Ledger::getMiners () const {

    map < string, MinerInfo > minerInfoMap;

    shared_ptr < SerializableSet < string >> miners = this->getObjectOrNull < SerializableSet < string >>( FormatLedgerKey::forMiners ());
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

    return this->getObjectOrNull < MinerURLMap >( FormatLedgerKey::forMinerURLs ());
}

//----------------------------------------------------------------//
shared_ptr < Schema > Ledger::getSchema ( Schema::Index schemaIndex ) const {

    string schemaString = this->getSchemaString ( schemaIndex );
    if ( schemaString.size () > 0 ) {
        shared_ptr < Schema > schema = make_shared < Schema >();
        FromJSONSerializer::fromJSONString ( *schema, schemaString );
        return schema;
    }
    return NULL;
}

//----------------------------------------------------------------//
Schema::Index Ledger::getSchemaCount () const {

    return this->getValue < Schema::Index >( FormatLedgerKey::forSchemaCount ());
}

//----------------------------------------------------------------//
Schema::Index Ledger::getSchemaIndex ( string schemaName ) const {

    LedgerKey KEY_FOR_SCHEMA_ALIAS = FormatLedgerKey::forAccountAlias ( schemaName );
    return this->getValueOrFallback < Schema::Index >( KEY_FOR_SCHEMA_ALIAS, Schema::NULL_INDEX );
}

//----------------------------------------------------------------//
string Ledger::getSchemaString ( Schema::Index schemaIndex ) const {

    return this->getValue < string >( FormatLedgerKey::forSchema ( schemaIndex ));
}

//----------------------------------------------------------------//
UnfinishedBlockList Ledger::getUnfinished () {

    shared_ptr < UnfinishedBlockList > unfinished = this->getObjectOrNull < UnfinishedBlockList >( FormatLedgerKey::forUnfinished ());
    return unfinished ? *unfinished : UnfinishedBlockList ();
}

//----------------------------------------------------------------//
void Ledger::incrementNonce ( const TransactionMaker& maker ) {

    u64 nonce = maker.getNonce ();
    string accountName = maker.getAccountName ();

    shared_ptr < Account > account = this->getAccount ( accountName );
    if ( account && ( account->mNonce <= nonce )) {
        Account updatedAccount = *account;
        updatedAccount.mNonce = nonce + 1;
        this->setAccount ( accountName, updatedAccount );
    }
}

//----------------------------------------------------------------//
bool Ledger::invoke ( string accountName, const AssetMethodInvocation& invocation ) {

    shared_ptr < AssetMethod > method = this->getMethod ( invocation.mMethodName );
    if ( !( method && ( method->mWeight == invocation.mWeight ) && ( method->mMaturity == invocation.mMaturity ))) return false;

    // make sure account exists
    Account::Index accountIndex = this->getAccountIndex ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return false;

    // TODO: this is brutally inefficient, but we're doing it for now. can add a cache of LuaContext objects later to speed things up.
    LuaContext lua ( method->mLua );
    return lua.invoke ( *this, accountName, *method, invocation );
}

//----------------------------------------------------------------//
bool Ledger::isAccountName ( string accountName ) {
    UNUSED ( accountName );

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
bool Ledger::keyPolicy ( string accountName, string policyName, const Policy* policy ) {
    UNUSED ( accountName );
    UNUSED ( policyName );
    UNUSED ( policy );

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
bool Ledger::newAccount ( string accountName, u64 balance, const CryptoKey& key ) {

    // check to see if there is already an alias for this account name
    LedgerKey KEY_FOR_ACCOUNT_ALIAS = FormatLedgerKey::forAccountAlias ( accountName );
    if ( this->hasKey ( KEY_FOR_ACCOUNT_ALIAS )) return false; // alias already exists

    // provision the account ID
    LedgerKey KEY_FOR_ACCOUNT_COUNT = FormatLedgerKey::forAccountCount ();
    Account::Index accountIndex = this->getValue < Account::Index >( KEY_FOR_ACCOUNT_COUNT );
    this->setValue < Account::Index >( KEY_FOR_ACCOUNT_COUNT, accountIndex + 1 ); // increment counter

    // store the account
    Account account;
    account.mIndex = accountIndex;
    account.mBalance = balance;
    account.mKeys [ MASTER_KEY_NAME ] = KeyAndPolicy ( key );
    
    this->setObject < Account >( FormatLedgerKey::forAccount ( accountIndex ), account );

    // store the key (for reverse lookup):
    string keyID = key.getKeyID ();
    assert ( keyID.size ());
    this->setObject < AccountKeyLookup >( FormatLedgerKey::forAccountKeyLookup ( keyID ), AccountKeyLookup ( accountIndex, MASTER_KEY_NAME ));

    // store the alias
    this->setValue < Account::Index >( KEY_FOR_ACCOUNT_ALIAS, accountIndex );
    this->setValue < string >( FormatLedgerKey::forAccountMember ( accountIndex, FormatLedgerKey::ACCOUNT_NAME ), accountName );

    return true;
}

//----------------------------------------------------------------//
bool Ledger::publishSchema ( string accountName, const Schema& schema, string schemaString ) {

    // TODO: check account permissions

    string schemaName = schema.mName;

    LedgerKey KEY_FOR_SCHEMA_ALIAS = FormatLedgerKey::forSchemaAlias ( schemaName );
    if ( this->hasValue ( KEY_FOR_SCHEMA_ALIAS )) return false;

    // provision the schema ID
    LedgerKey KEY_FOR_SCHEMA_COUNT = FormatLedgerKey::forSchemaCount ();
    Schema::Index schemaIndex = this->getValue < Schema::Index >( KEY_FOR_SCHEMA_COUNT );
    this->setValue < Schema::Index >( KEY_FOR_SCHEMA_COUNT, schemaIndex + 1 ); // increment counter
    
    this->setValue < string >( FormatLedgerKey::forSchema ( schemaIndex ), schemaString );
    
    // store the alias
    this->setValue < Account::Index >( KEY_FOR_SCHEMA_ALIAS, schemaIndex );

    Schema::Definitions::const_iterator definitionIt = schema.mDefinitions.cbegin ();
    for ( ; definitionIt != schema.mDefinitions.cend (); ++definitionIt ) {
    
        string typeName = definitionIt->first;
        const AssetDefinition& definition = definitionIt->second;
    
        // store the definition for easy access later
        LedgerKey KEY_FOR_ASSET_DEFINITION = FormatLedgerKey::forAssetDefinition ( typeName );
        if ( this->hasValue ( KEY_FOR_ASSET_DEFINITION )) return false; // can't overwrite definitions
        this->setObject < AssetDefinition >( KEY_FOR_ASSET_DEFINITION, definition );
    }

    Schema::Methods::const_iterator methodIt = schema.mMethods.cbegin ();
    for ( ; methodIt != schema.mMethods.cend (); ++methodIt ) {
    
        string methodName = methodIt->first;
        const AssetMethod& method = methodIt->second;
        
        // store the method for easy access later
        LedgerKey KEY_FOR_METHOD = FormatLedgerKey::forMethod ( methodName );
        if ( this->hasValue ( KEY_FOR_METHOD )) return false; // can't overwrite methods
        this->setObject < AssetMethod >( KEY_FOR_METHOD, method );
    }

    LuaContext lua ( schema.mLua );
    lua.invoke ( *this, accountName );
    
    return true;
}

//----------------------------------------------------------------//
bool Ledger::registerMiner ( string accountName, string keyName, string url ) {

    AccountKey accountKey = this->getAccountKey ( accountName, keyName );
    if ( accountKey ) {

        shared_ptr < Account > account = accountKey.mAccount;
        Account::Index accountIndex = account->mIndex;

        this->setObject < MinerInfo >(
            FormatLedgerKey::forAccountMember ( accountIndex, FormatLedgerKey::ACCOUNT_MINER_INFO ),
            MinerInfo ( accountIndex, url, accountKey.mKeyAndPolicy->mKey )
        );
        
        // TODO: find an efficient way to do all this
        
        LedgerKey KEY_FOR_MINERS = FormatLedgerKey::forMiners ();
        shared_ptr < SerializableSet < string >> miners = this->getObjectOrNull < SerializableSet < string >>( KEY_FOR_MINERS );
        assert ( miners );
        miners->insert ( accountName );
        this->setObject < SerializableSet < string >>( KEY_FOR_MINERS, *miners );
        
        LedgerKey KEY_FOR_MINER_URLS = FormatLedgerKey::forMinerURLs ();
        shared_ptr < SerializableMap < string, string >> minerURLs = this->getObjectOrNull < SerializableMap < string, string >>( KEY_FOR_MINER_URLS );
        assert ( minerURLs );
        ( *minerURLs )[ accountName ] = url;
        this->setObject < SerializableMap < string, string >>( KEY_FOR_MINER_URLS, *minerURLs );

        return true;
    }
    return false;
}

//----------------------------------------------------------------//
bool Ledger::renameAccount ( string accountName, string revealedName, Digest nameHash, Digest nameSecret ) {
    UNUSED ( nameHash );
    UNUSED ( nameSecret );

    // nameHash <- SHA256 ( "<new name>" )
    // nameSecret <- SHA256 ( "<current name>:<new name>" )

    // if provided, nameHash and nameSecret may be used to reduce the chances of an account
    // name being intercepted and registered by an attacker.
    
    // nameHash establishes the uniqueness of an account name without revealing it. nameSecret
    // binds the account name to the registrant's account. using both ensures that the registrant
    // really knows the requested account name. in other words, the registrant's own account
    // name acts like a salt: an attacker would have to find a match that produced both the
    // nameHash *and* the nameSecret (derived from their own account name). this, nameHash
    // is shared, but nameSecret is unique to every account applying for the name.
    
    // the nameHash is used to create a decollider table, which records all accounts applying
    // for the name. the nameSecret is stored inside the account, along with a timestamp.
    
    // when a plaintext name is revealed, the decollider table is checked. each colliding
    // account may then be checked to see if its nameSecret is correct. whichever account
    // has the correct namesecret and the earliest timestamp is the rightful claimant
    // of the name.
    
    // once a claimant is found, the decollider table may be updated to reflect the change
    // in status and indicate the winner (although the winner must also submit a rename
    // transaction to claim the name).
    
    // make sure the revealed name is valid
    if ( Ledger::isChildName ( revealedName )) return false; // new names must not begin with a '~'
    if ( !Ledger::isAccountName ( revealedName )) return false; // make sure it's a valid account name
    
    Account::Index accountIndex = this->getAccountIndex ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return false;
    
    this->setValue < Account::Index >( FormatLedgerKey::forAccountAlias ( revealedName ), accountIndex );
    this->setValue < string >( FormatLedgerKey::forAccountMember ( accountIndex, FormatLedgerKey::ACCOUNT_NAME ), revealedName );
 
    return false;
}

//----------------------------------------------------------------//
void Ledger::reset () {

    this->clear ();
    this->setObject < SerializableSet < string >>( FormatLedgerKey::forMiners (), SerializableSet < string > ());
    this->setObject < SerializableMap < string, string >>( FormatLedgerKey::forMinerURLs (), SerializableMap < string, string > ());
    this->setValue < Asset::Index >( FormatLedgerKey::forAccountCount (), 0 );
    this->setValue < Schema::Index >( FormatLedgerKey::forSchemaCount (), 0 );
    this->setValue < Asset::Index >( FormatLedgerKey::forAssetCount (), 0 );
}

//----------------------------------------------------------------//
bool Ledger::sendVOL ( string accountName, string recipientName, u64 amount ) {

    shared_ptr < Account > account      = this->getAccount ( accountName );
    shared_ptr < Account > recipient    = this->getAccount ( recipientName );

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

    Account::Index accountIndex = this->getAccountIndex ( accountName );
    assert ( accountIndex != Account::NULL_INDEX );

    this->setObject < Account >( FormatLedgerKey::forAccount ( accountIndex ), account );
}

//----------------------------------------------------------------//
bool Ledger::setAssetFieldValue ( Asset::Index index, string fieldName, const AssetFieldValue& field ) {

    // make sure the asset exists
    LedgerKey KEY_FOR_ASSET_TYPE = FormatLedgerKey::forAssetMember ( index, FormatLedgerKey::ASSET_TYPE );
    if ( !this->hasValue ( KEY_FOR_ASSET_TYPE )) return false;

    string assetType = this->getValue < string >( KEY_FOR_ASSET_TYPE );

    // make sure the field exists
    LedgerKey KEY_FOR_ASSET_DEFINITION = FormatLedgerKey::forAssetDefinition ( assetType );
    shared_ptr < AssetDefinition > assetDefinition = this->getObjectOrNull < AssetDefinition >( KEY_FOR_ASSET_DEFINITION );
    if ( !assetDefinition ) return false;
    if ( !assetDefinition->hasMutableField ( fieldName, field.getType ())) return false;

//    // set the field
//    string keyForAssetField = Ledger::formatKeyForAssetField ( identifier, fieldName );
//
//    switch ( field.mType ) {
//        case AssetTemplateField::Type::NUMERIC:
//            this->setValue < double >( keyForAssetField, field.mValue.mNumeric );
//            break;
//        case AssetTemplateField::Type::STRING:
//            this->setValue < string >( keyForAssetField, field.mValue.mString );
//            break;
//    }

    return true;
}

//----------------------------------------------------------------//
void Ledger::setBlock ( const Block& block ) {
    assert ( block.mHeight == this->getVersion ());
    this->setObject < Block >( FormatLedgerKey::forBlock (), block );
}

//----------------------------------------------------------------//
void Ledger::setEntropyString ( string entropy ) {

    this->setValue < string >( FormatLedgerKey::forEntropy (), entropy );
}

//----------------------------------------------------------------//
bool Ledger::setIdentity ( string identity ) {

    LedgerKey KEY_FOR_IDENTITY = FormatLedgerKey::forIdentity ();
    if ( this->hasValue ( KEY_FOR_IDENTITY )) return false;
    this->setValue < string >( KEY_FOR_IDENTITY, identity );
    return true;
}

//----------------------------------------------------------------//
void Ledger::setMinerInfo ( string accountName, const MinerInfo& minerInfo ) {

    Account::Index accountIndex = this->getAccountIndex ( accountName );
    assert ( accountIndex != Account::NULL_INDEX );

    this->setObject < MinerInfo >( FormatLedgerKey::forAccountMember ( accountIndex, FormatLedgerKey::ACCOUNT_MINER_INFO ), minerInfo );
}

//----------------------------------------------------------------//
void Ledger::setUnfinished ( const UnfinishedBlockList& unfinished ) {

    this->setObject < UnfinishedBlockList >( FormatLedgerKey::forUnfinished (), unfinished );
}

//----------------------------------------------------------------//
bool Ledger::sponsorAccount ( string sponsorName, string suffix, u64 grant, const CryptoKey& key ) {

    if ( Ledger::isChildName ( sponsorName )) return false;
    if ( !Ledger::isSuffix ( suffix )) return false;
    
    // the child name will be prepended following a tilde: "~<sponsorName>.<childSuffix>"
    // i.e. "~maker.000.000.000"
    // this prevents it from sponsoring any new accounts until it is renamed.

    string childName = Format::write ( "~%s.%s", sponsorName.c_str (), suffix.c_str ());
    assert ( Ledger::isChildName ( childName ));

    shared_ptr < Account > account = this->getAccount ( sponsorName );
    if ( account && ( account->mBalance >= grant )) {

        if ( !this->newAccount ( childName, grant, key )) return false;

        account->mBalance -= grant;
        this->setAccount ( sponsorName, *account );

        return true;
    }
    return false;
}

//----------------------------------------------------------------//
bool Ledger::verify ( const AssetMethodInvocation& invocation ) const {

    shared_ptr < AssetMethod > method = this->getMethod ( invocation.mMethodName );
    return ( method && ( method->mWeight == invocation.mWeight ) && ( method->mMaturity == invocation.mMaturity ));
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
