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
bool Ledger::affirmKey ( string accountName, string makerKeyName, string keyName, const CryptoKey& key, const Policy* policy ) {

    string keyID = key.getKeyID ();
    if ( keyID.size ()) return false;

    shared_ptr < Account > account = this->getAccount ( accountName );
    if ( account ) {

        const LedgerKey KEY_FOR_ACCOUNT_KEY_LOOKUP = FormatLedgerKey::forAccountKeyLookup ( keyID );
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
            this->setAccount ( accountName, *account );
            
            this->setObject < AccountKeyLookup >( KEY_FOR_ACCOUNT_KEY_LOOKUP, AccountKeyLookup ( account->mIndex, keyName ));
            
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------//
bool Ledger::awardAsset ( const Schema& schema, string accountName, string assetType, int quantity ) {

    if ( quantity == 0 ) return true;

    if ( !schema.getDefinitionOrNull ( assetType )) return false;

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
        this->setValue < Asset::Index >( FormatLedgerKey::forAssetRecordMember ( accountTailIndex, FormatLedgerKey::ASSET_NEXT ), firstAssetIndex );
    }
    Asset::Index firstElementPrev = accountTailIndex;
    accountTailIndex = ( firstAssetIndex + quantity ) - 1;
    this->setValue < Asset::Index >( KEY_FOR_ACCOUNT_TAIL, accountTailIndex );
    
    // now add the assets
    for ( Asset::Index i = firstAssetIndex; i <= accountTailIndex; ++i ) {
        
        this->setValue < Account::Index >( FormatLedgerKey::forAssetRecordMember ( i, FormatLedgerKey::ASSET_OWNER_INDEX ), accountIndex );
        this->setValue < string >( FormatLedgerKey::forAssetRecordMember ( i, FormatLedgerKey::ASSET_TYPE ), assetType );
        
        string KEY_FOR_ASSET_PREV = FormatLedgerKey::forAssetRecordMember ( i, FormatLedgerKey::ASSET_PREV );
        string KEY_FOR_ASSET_NEXT = FormatLedgerKey::forAssetRecordMember ( i, FormatLedgerKey::ASSET_NEXT );
        
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

    if ( !this->newAccount ( accountName, balance, key, Policy (), Policy ())) return false;
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
shared_ptr < Asset > Ledger::getAsset ( const Schema& schema, Asset::Index index ) const {

    string KEY_FOR_ASSET_OWNER_INDEX = FormatLedgerKey::forAssetRecordMember ( index, FormatLedgerKey::ASSET_OWNER_INDEX );
    if ( !this->hasValue ( KEY_FOR_ASSET_OWNER_INDEX )) return NULL;
    Account::Index ownerIndex = this->getValue < Account::Index >( KEY_FOR_ASSET_OWNER_INDEX );

    LedgerKey KEY_FOR_ASSET_TYPE = FormatLedgerKey::forAssetRecordMember ( index, FormatLedgerKey::ASSET_TYPE );
    string typeName = this->getValue < string >( KEY_FOR_ASSET_TYPE );

    const AssetDefinition* assetDefinition = schema.getDefinitionOrNull ( typeName );
    if ( !assetDefinition ) return NULL;
    
    shared_ptr < Asset > asset = make_shared < Asset >();
    asset->mType    = typeName;
    asset->mIndex   = index;
    asset->mOwner   = this->getAccountName ( ownerIndex );
    
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
SerializableList < Asset > Ledger::getInventory ( const Schema& schema, string accountName ) const {

    SerializableList < Asset > assets;

    Account::Index accountIndex = this->getAccountIndex ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return assets;

    LedgerKey KEY_FOR_ACCOUNT_HEAD = FormatLedgerKey::forAccountMember ( accountIndex, FormatLedgerKey::ACCOUNT_HEAD );
    Asset::Index cursor = this->getValueOrFallback < Asset::Index >( KEY_FOR_ACCOUNT_HEAD, Asset::NULL_INDEX );
    
    while ( cursor != Asset::NULL_INDEX ) {
    
        shared_ptr < Asset > asset = this->getAsset ( schema, cursor );
        assert ( asset );
        assets.push_back ( *asset );
    
        cursor = this->getValue < Asset::Index >( FormatLedgerKey::forAssetRecordMember ( cursor, FormatLedgerKey::ASSET_NEXT ));
    }
    return assets;
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
void Ledger::getSchema ( Schema& schema ) const {

    string schemaString = this->getSchemaString ();
    if ( schemaString.size () > 0 ) {
        FromJSONSerializer::fromJSONString ( schema, schemaString );
    }
}

//----------------------------------------------------------------//
string Ledger::getSchemaString () const {

    return this->getValue < string >( FormatLedgerKey::forSchema ());
}

//----------------------------------------------------------------//
string Ledger::getTransactionNote ( string accountName, u64 nonce ) const {

    Account::Index accountIndex = this->getAccountIndex ( accountName );
    if ( accountIndex != Account::NULL_INDEX ) {

        LedgerKey KEY_FOR_ACCOUNT_TRANSACTION_NOTE = FormatLedgerKey::forAccountTransactionNote ( accountIndex, nonce );
        return this->getValueOrFallback < string >( KEY_FOR_ACCOUNT_TRANSACTION_NOTE, "" );
    }
    return "";
}

//----------------------------------------------------------------//
UnfinishedBlockList Ledger::getUnfinished () {

    shared_ptr < UnfinishedBlockList > unfinished = this->getObjectOrNull < UnfinishedBlockList >( FormatLedgerKey::forUnfinished ());
    return unfinished ? *unfinished : UnfinishedBlockList ();
}

//----------------------------------------------------------------//
void Ledger::incrementNonce ( const TransactionMaker& maker, string note ) {

    u64 nonce = maker.getNonce ();
    string accountName = maker.getAccountName ();

    shared_ptr < Account > account = this->getAccount ( accountName );
    if ( account && ( account->mNonce == nonce )) {
    
        LedgerKey KEY_FOR_ACCOUNT_TRANSACTION_NOTE = FormatLedgerKey::forAccountTransactionNote ( account->mIndex, nonce );
        this->setValue < string >( KEY_FOR_ACCOUNT_TRANSACTION_NOTE, note );
    
        Account updatedAccount = *account;
        updatedAccount.mNonce = nonce + 1;
        this->setAccount ( accountName, updatedAccount );
    }
}

//----------------------------------------------------------------//
void Ledger::init () {

    this->clear ();
    this->setObject < SerializableSet < string >>( FormatLedgerKey::forMiners (), SerializableSet < string > ());
    this->setObject < SerializableMap < string, string >>( FormatLedgerKey::forMinerURLs (), SerializableMap < string, string > ());
    this->setValue < Asset::Index >( FormatLedgerKey::forAccountCount (), 0 );
    this->setValue < Asset::Index >( FormatLedgerKey::forAssetCount (), 0 );
    this->setValue < string >( FormatLedgerKey::forSchema (), "" );
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
bool Ledger::newAccount ( string accountName, u64 balance, const CryptoKey& key, const Policy& keyPolicy, const Policy& accountPolicy ) {

    // check to see if there is already an alias for this account name
    LedgerKey KEY_FOR_ACCOUNT_ALIAS = FormatLedgerKey::forAccountAlias ( accountName );
    if ( this->hasKey ( KEY_FOR_ACCOUNT_ALIAS )) return false; // alias already exists

    // provision the account ID
    LedgerKey KEY_FOR_ACCOUNT_COUNT = FormatLedgerKey::forAccountCount ();
    Account::Index accountIndex = this->getValue < Account::Index >( KEY_FOR_ACCOUNT_COUNT );
    this->setValue < Account::Index >( KEY_FOR_ACCOUNT_COUNT, accountIndex + 1 ); // increment counter

    // store the account
    Account account;
    account.mPolicy = accountPolicy;
    account.mIndex = accountIndex;
    account.mName = accountName;
    account.mBalance = balance;
    account.mKeys [ MASTER_KEY_NAME ] = KeyAndPolicy ( key, keyPolicy );
    
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
bool Ledger::publishSchema ( string accountName, const Schema& schema ) {
    UNUSED ( accountName );

    // TODO: check account permissions

    Schema currentSchema;

    LedgerKey KEY_FOR_SCHEMA = FormatLedgerKey::forSchema ();

    string schemaString = this->getValue < string >( FormatLedgerKey::forSchema ());
    if ( schemaString.size () > 0 ) {
        FromJSONSerializer::fromJSONString ( currentSchema, schemaString );
    }
    currentSchema.compose ( schema );
    
    this->setObject < Schema >( KEY_FOR_SCHEMA, schema );
    
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
    // nameHash *and* the nameSecret (derived from their own account name). thus, nameHash
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
    
    shared_ptr < Account > account = this->getAccount ( accountName );
    if ( !account ) return false;
    
    this->setValue < Account::Index >( FormatLedgerKey::forAccountAlias ( revealedName ), account->mIndex );
    this->setValue < string >( FormatLedgerKey::forAccountMember ( accountIndex, FormatLedgerKey::ACCOUNT_NAME ), revealedName );
 
    account->mName = revealedName;
    this->setObject < Account >( FormatLedgerKey::forAccount ( account->mIndex ), *account );
 
    return true;
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
bool Ledger::setAssetFieldValue ( const Schema& schema, Asset::Index index, string fieldName, const AssetFieldValue& field ) {

    // make sure the asset exists
    LedgerKey KEY_FOR_ASSET_TYPE = FormatLedgerKey::forAssetRecordMember ( index, FormatLedgerKey::ASSET_TYPE );
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
    this->setObject < Block >( FormatLedgerKey::forBlock (), block );
}

//----------------------------------------------------------------//
void Ledger::setEntitlements ( string name, const Entitlements& entitlements ) {

    LedgerKey KEY_FOR_ENTITLEMENTS = FormatLedgerKey::forEntitlements ( name );
    this->setObject < Entitlements >( KEY_FOR_ENTITLEMENTS, entitlements );
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
bool Ledger::setKeyBequest ( string accountName, string keyName, const Policy* bequest ) {

    shared_ptr < Account > account = this->getAccount ( accountName );
    if ( account ) {

        const KeyAndPolicy* keyAndPolicy = account->getKeyAndPolicyOrNull ( keyName );
        if ( !keyAndPolicy ) return false;
        
        if ( bequest ) {
            if ( !this->isMoreRestrictivePolicy < KeyEntitlements >( *bequest, keyAndPolicy->mPolicy )) return false;
        }
        account->mKeys [ keyName ].mBequest = bequest ? make_shared < Policy >( *bequest ) : NULL;
        this->setAccount ( accountName, *account );
        
        return true;
    }
    return false;
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
bool Ledger::sponsorAccount ( string sponsorName, string keyName, string suffix, u64 grant, const CryptoKey& key, const Policy* keyPolicy, const Policy* accountPolicy ) {

    if ( Ledger::isChildName ( sponsorName )) return false;
    if ( !Ledger::isSuffix ( suffix )) return false;
    
    // the child name will be prepended with the sponsor name following a tilde: "~<sponsorName>.<childSuffix>"
    // i.e. "~maker.000.000.000"
    // this prevents it from sponsoring any new accounts until it is renamed.

    string childName = Format::write ( "~%s.%s", sponsorName.c_str (), suffix.c_str ());
    assert ( Ledger::isChildName ( childName ));

    shared_ptr < Account > account = this->getAccount ( sponsorName );
    if ( account && ( account->mBalance >= grant )) {

        const KeyAndPolicy* sponsorKeyAndPolicy = account->getKeyAndPolicyOrNull ( keyName );
        if ( !sponsorKeyAndPolicy ) return false;

        const Policy& sponsorKeyPolicy = sponsorKeyAndPolicy->mPolicy;
        Entitlements sponsorKeyEntitlements = this->getEntitlements < KeyEntitlements >( sponsorKeyPolicy );
        if ( !KeyEntitlements::canOpenAccount ( sponsorKeyEntitlements )) return false;

        const Policy* keyBequest = this->resolveBequest < KeyEntitlements >( sponsorKeyPolicy, sponsorKeyAndPolicy->getBequest (), keyPolicy );
        if ( !keyBequest ) return false;
        
        const Policy* accountBequest = this->resolveBequest < AccountEntitlements >( account->mPolicy, account->getBequest (), accountPolicy );
        if ( !accountBequest ) return false;

        if ( !this->newAccount ( childName, grant, key, *keyBequest, *accountBequest )) return false;

        account->mBalance -= grant;
        this->setAccount ( sponsorName, *account );

        return true;
    }
    return false;
}

//----------------------------------------------------------------//
bool Ledger::upgradeAsset ( const Schema& schema, Account::Index accountIndex, Asset::Index assetIndex, string upgradeType ) {

    string KEY_FOR_ASSET_OWNER_INDEX = FormatLedgerKey::forAssetRecordMember ( assetIndex, FormatLedgerKey::ASSET_OWNER_INDEX );
    if ( !this->hasValue ( KEY_FOR_ASSET_OWNER_INDEX )) return NULL;

    Account::Index ownerIndex = this->getValue < Account::Index >( KEY_FOR_ASSET_OWNER_INDEX );
    if ( ownerIndex != accountIndex ) return false;

    LedgerKey KEY_FOR_ASSET_TYPE = FormatLedgerKey::forAssetRecordMember ( assetIndex, FormatLedgerKey::ASSET_TYPE );
    string typeName = this->getValue < string >( KEY_FOR_ASSET_TYPE );

    if ( !schema.canUpgrade ( typeName, upgradeType )) return false;

    this->setValue < string >( KEY_FOR_ASSET_TYPE, upgradeType );

    return true;
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
