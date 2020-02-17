// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Asset.h>
#include <volition/AssetMethod.h>
#include <volition/AssetMethodInvocation.h>
#include <volition/Block.h>
#include <volition/Format.h>
#include <volition/Ledger.h>
#include <volition/LedgerODBM.h>
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

    LedgerKey KEY_FOR_GLOBAL_ASSET_COUNT = FormatLedgerKey::forGlobalAssetCount ();
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

    return this->getObjectOrNull < Account >( FormatLedgerKey::forAccount_body ( index ));
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

    return this->getValueOrFallback < string >( FormatLedgerKey::forAccount_name ( accountIndex ), "" );
}

//----------------------------------------------------------------//
shared_ptr < Asset > Ledger::getAsset ( const Schema& schema, Asset::Index index ) const {

    AssetODBM assetODBM ( *this, index );
    if ( !assetODBM.mOwner.exists ()) return NULL;

    const AssetDefinition* assetDefinition = schema.getDefinitionOrNull ( assetODBM.mType.get ());
    if ( !assetDefinition ) return NULL;
    
    shared_ptr < Asset > asset = make_shared < Asset >();
    asset->mType    = assetODBM.mType.get ();
    asset->mIndex   = assetODBM.mIndex;
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

    AccountODBM accountODBM ( *this, this->getAccountIndex ( accountName ));
    if ( accountODBM.mIndex == Account::NULL_INDEX ) return assets;

    size_t assetCount = accountODBM.mAssetCount.get ();
    
    for ( size_t i = 0; i < assetCount; ++i ) {
    
        Asset::Index assetIndex = accountODBM.getInventoryField ( i ).get ();
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

    return this->getObjectOrNull < MinerInfo >( FormatLedgerKey::forAccount_minerInfo ( accountIndex ));
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

        LedgerKey KEY_FOR_ACCOUNT_TRANSACTION_NOTE = FormatLedgerKey::forAccount_transactionNoteField ( accountIndex, nonce );
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
void Ledger::incrementNonce ( Account::Index index, u64 nonce, string note ) {

    shared_ptr < Account > account = this->getAccount ( index );
    assert ( account );
    assert ( account->mNonce == nonce );
    
    LedgerKey KEY_FOR_ACCOUNT_TRANSACTION_NOTE = FormatLedgerKey::forAccount_transactionNoteField ( index, nonce );
    this->setValue < string >( KEY_FOR_ACCOUNT_TRANSACTION_NOTE, note );

    Account updatedAccount = *account;
    updatedAccount.mNonce = nonce + 1;
    this->setAccount ( updatedAccount );
}

//----------------------------------------------------------------//
void Ledger::init () {

    this->clear ();
    this->setObject < SerializableSet < string >>( FormatLedgerKey::forMiners (), SerializableSet < string > ());
    this->setObject < SerializableMap < string, string >>( FormatLedgerKey::forMinerURLs (), SerializableMap < string, string > ());
    this->setValue < Asset::Index >( FormatLedgerKey::forGlobalAccountCount (), 0 );
    this->setValue < Asset::Index >( FormatLedgerKey::forGlobalAssetCount (), 0 );
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
bool Ledger::newAccount ( string accountName, u64 balance, string keyName, const CryptoKey& key, const Policy& keyPolicy, const Policy& accountPolicy ) {

    // check to see if there is already an alias for this account name
    LedgerKey KEY_FOR_ACCOUNT_ALIAS = FormatLedgerKey::forAccountAlias ( accountName );
    if ( this->hasKey ( KEY_FOR_ACCOUNT_ALIAS )) return false; // alias already exists

    // provision the account ID
    LedgerKey KEY_FOR_GLOBAL_ACCOUNT_COUNT = FormatLedgerKey::forGlobalAccountCount ();
    Account::Index accountIndex = this->getValue < Account::Index >( KEY_FOR_GLOBAL_ACCOUNT_COUNT );
    this->setValue < Account::Index >( KEY_FOR_GLOBAL_ACCOUNT_COUNT, accountIndex + 1 ); // increment counter

    // store the account
    Account account;
    account.mPolicy = accountPolicy;
    account.mIndex = accountIndex;
    account.mName = accountName;
    account.mBalance = balance;
    account.mKeys [ MASTER_KEY_NAME ] = KeyAndPolicy ( key, keyPolicy );
    
    this->setObject < Account >( FormatLedgerKey::forAccount_body ( accountIndex ), account );

    // store the key (for reverse lookup):
    string keyID = key.getKeyID ();
    assert ( keyID.size ());
    this->setObject < AccountKeyLookup >( FormatLedgerKey::forAccountKeyLookup ( keyID ), AccountKeyLookup ( accountIndex, keyName ));

    // store the alias
    this->setValue < Account::Index >( KEY_FOR_ACCOUNT_ALIAS, accountIndex );
    this->setValue < string >( FormatLedgerKey::forAccount_name ( accountIndex ), accountName );

    return true;
}

//----------------------------------------------------------------//
bool Ledger::publishSchema ( const Schema& schema ) {

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
            FormatLedgerKey::forAccount_minerInfo ( accountIndex ),
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
    
    shared_ptr < Account > account = this->getAccount ( accountName );
    if ( !account ) return false;
    
    LedgerKey KEY_FOR_ACCOUNT_ALIAS = FormatLedgerKey::forAccountAlias ( revealedName );
    
    // check to see if the alias already exists
    if ( this->hasValue ( KEY_FOR_ACCOUNT_ALIAS )) {
        // if it does, then it should also belong to this account
        Account::Index aliasIndex = this->getValue < Account::Index >( KEY_FOR_ACCOUNT_ALIAS );
        if ( aliasIndex != account->mIndex ) return false;
    }
    else {
        this->setValue < Account::Index >(KEY_FOR_ACCOUNT_ALIAS, account->mIndex );
    }
    this->setValue < string >( FormatLedgerKey::forAccount_name ( account->mIndex ), revealedName );
 
    account->mName = revealedName;
    this->setObject < Account >( FormatLedgerKey::forAccount_body ( account->mIndex ), *account );

    return true;
}

//----------------------------------------------------------------//
bool Ledger::sendAssets ( string accountName, string recipientName, const string* assetIdentifiers, size_t totalAssets ) {

    AccountODBM senderODBM ( *this, this->getAccountIndex ( accountName ));
    AccountODBM receiverODBM ( *this, this->getAccountIndex ( recipientName ));

    if ( senderODBM.mIndex == Account::NULL_INDEX ) return false;
    if ( receiverODBM.mIndex == Account::NULL_INDEX ) return false;

    size_t senderAssetCount = senderODBM.mAssetCount.get ( 0 );
    size_t receiverAssetCount = receiverODBM.mAssetCount.get ( 0 );

    for ( size_t i = 0; i < totalAssets; ++i, --senderAssetCount, ++receiverAssetCount ) {
        
        AssetODBM assetODBM ( *this, AssetID::decode ( assetIdentifiers [ i ]));
        if ( assetODBM.mIndex == Asset::NULL_INDEX ) return false;
        if ( assetODBM.mOwner.get () != senderODBM.mIndex ) return false;
        
        // fill the asset's original position by swapping in the tail
        size_t position = assetODBM.mPosition.get ();
        if ( position < senderAssetCount ) {
            LedgerFieldODBM < Asset::Index > senderInventoryField = senderODBM.getInventoryField ( position );
            LedgerFieldODBM < Asset::Index > senderInventoryTailField = senderODBM.getInventoryField ( senderAssetCount - 1 );
            
            AssetODBM tailAssetODBM ( *this, senderInventoryTailField.get ());
            tailAssetODBM.mPosition.set ( position );
            senderInventoryField.set ( tailAssetODBM.mIndex );
        }
        
        // transfer asset ownership to the receiver
        assetODBM.mOwner.set ( receiverODBM.mIndex );
        assetODBM.mPosition.set ( receiverAssetCount );
        receiverODBM.getInventoryField ( assetODBM.mPosition.get ()).set ( assetODBM.mIndex );
    }
    
    senderODBM.mAssetCount.set ( senderAssetCount );
    receiverODBM.mAssetCount.set ( receiverAssetCount );

    return true;
}

//----------------------------------------------------------------//
void Ledger::setAccount ( const Account& account ) {

    assert ( account.mIndex != Account::NULL_INDEX );
    this->setObject < Account >( FormatLedgerKey::forAccount_body ( account.mIndex ), account );
}

//----------------------------------------------------------------//
bool Ledger::setAssetFieldValue ( const Schema& schema, Asset::Index index, string fieldName, const AssetFieldValue& field ) {

    // make sure the asset exists
    LedgerKey KEY_FOR_ASSET_TYPE = FormatLedgerKey::forAsset_type ( index );
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

    this->setObject < UnfinishedBlockList >( FormatLedgerKey::forUnfinished (), unfinished );
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
