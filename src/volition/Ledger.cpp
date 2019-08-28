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

    string keyInfoPrefix = KEY_ID + keyID;
    shared_ptr < KeyInfo > keyInfo = this->getJSONSerializableObject < KeyInfo >( keyInfoPrefix );

    if ( keyInfo && ( keyInfo->mAccountName != accountName )) return false;

    shared_ptr < Account > account = this->getAccount ( accountName );
    if ( account ) {

        if ( key ) {
            
            Account updatedAccount = *account;
            updatedAccount.mKeys [ keyName ] = KeyAndPolicy ( key );
            this->setAccount ( accountName, updatedAccount );
            
            this->setJSONSerializableObject < KeyInfo >( keyInfoPrefix, KeyInfo ( accountName, keyName ));
            
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------//
bool Ledger::awardAsset ( string accountName, string assetType, int quantity ) {

    if ( quantity == 0 ) return true;

    string keyForAssetCounter = Ledger::formatKeyForAssetCounter ();
    Asset::Index firstAssetIndex = this->getValue < Asset::Index >( keyForAssetCounter );

    // update head
    string keyForAccountHead = Ledger::formatKeyForAccount( accountName, ACCOUNT_HEAD );
    Asset::Index accountHeadIndex = this->getValueOrFallback < Asset::Index >( keyForAccountHead, Asset::NULL_ASSET_ID );

    if ( accountHeadIndex == Asset::NULL_ASSET_ID ) {
        this->setValue < Asset::Index >( keyForAccountHead, firstAssetIndex );
    }

    // update tail
    string keyForAccountTail = Ledger::formatKeyForAccount( accountName, ACCOUNT_TAIL );
    Asset::Index accountTailIndex = this->getValueOrFallback < Asset::Index >( keyForAccountTail, Asset::NULL_ASSET_ID );
    
    if ( accountTailIndex != Asset::NULL_ASSET_ID ) {
        this->setValue < Asset::Index >( Ledger::formatKeyForAsset ( accountTailIndex, ASSET_NEXT ), firstAssetIndex );
    }
    Asset::Index firstElementPrev = accountTailIndex;
    accountTailIndex = ( firstAssetIndex + quantity ) - 1;
    this->setValue < Asset::Index >( keyForAccountTail, accountTailIndex );
    
    // now add the assets
    for ( Asset::Index i = firstAssetIndex; i <= accountTailIndex; ++i ) {
        
        this->setValue < string >( Ledger::formatKeyForAsset ( i, ASSET_OWNER ), accountName );
        this->setValue < string >( Ledger::formatKeyForAsset ( i, ASSET_TYPE ), assetType );
        
        string keyForAssetPrev = Ledger::formatKeyForAsset ( i, ASSET_PREV );
        string keyForAssetNext = Ledger::formatKeyForAsset ( i, ASSET_NEXT );
        
        if ( i == firstAssetIndex ) {
            this->setValue < Asset::Index >( keyForAssetPrev, firstElementPrev );
        }
        else {
            this->setValue < Asset::Index >( keyForAssetPrev, i - 1 );
        }
        
        if ( i < accountTailIndex ) {
            this->setValue < Asset::Index >( keyForAssetNext, i + 1 );
        }
        else {
            this->setValue < Asset::Index >( keyForAssetNext, Asset::NULL_ASSET_ID );
        }
    }
    
    // increment total asset count
    this->setValue < size_t >( keyForAssetCounter, accountTailIndex + 1 );

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
string Ledger::formatKeyForAccount ( string accountName, string member ) {

    assert ( member.size () > 0 );
    return Format::write ( "asset.%s.%s", accountName.c_str (), member.c_str ());
}

//----------------------------------------------------------------//
string Ledger::formatKeyForAsset ( Asset::Index index, string member ) {

    assert ( member.size () > 0 );
    return Format::write ( "asset.%d.%s", index, member.c_str ());
}

//----------------------------------------------------------------//
string Ledger::formatKeyForAssetCounter () {

    return Format::write ( "asset.count" );
}

//----------------------------------------------------------------//
string Ledger::formatKeyForAssetDefinition ( string assetType ) {

    return Format::write ( "asset.%s.definition", assetType.c_str ());
}

//----------------------------------------------------------------//
string Ledger::formatKeyForAssetField ( Asset::Index index, string fieldName ) {

    return Format::write ( "asset.%d.fields.%s", index, fieldName.c_str ());
}

//----------------------------------------------------------------//
string Ledger::formatKeyForAssetMethod ( string methodName ) {

    return Format::write ( "method.%s", methodName.c_str ());
}

//----------------------------------------------------------------//
string Ledger::formatKeyForSchemaCount () {

    return "schema.count";
}

//----------------------------------------------------------------//
string Ledger::formatSchemaKey ( int schemaCount ) {

    return Format::write ( "schema.%d", schemaCount );
}

//----------------------------------------------------------------//
string Ledger::formatSchemaKey ( string schemaName ) {

    return Format::write ( "schema.%s", schemaName.c_str ());
}

//----------------------------------------------------------------//
bool Ledger::genesisMiner ( string accountName, u64 amount, string keyName, const CryptoKey& key, string url ) {

    Account account;

    account.mBalance = amount;
    account.mKeys [ keyName ] = KeyAndPolicy ( key );
    this->setAccount ( accountName, account );

    string keyID = key.getKeyID ();
    assert ( keyID.size ());

    this->setJSONSerializableObject < KeyInfo >( KEY_ID + keyID, KeyInfo ( accountName, keyName ));

    return this->registerMiner ( accountName, keyName, url );
}

//----------------------------------------------------------------//
shared_ptr < Account > Ledger::getAccount ( string accountName ) const {

    return this->getJSONSerializableObject < Account >( prefixKey ( ACCOUNT, accountName ));
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
shared_ptr < Asset > Ledger::getAsset ( Asset::Index index ) const {

    string keyForAssetOwner = Ledger::formatKeyForAsset ( index, ASSET_OWNER );
    if ( !this->hasValue ( keyForAssetOwner )) return NULL;

    string keyForAssetType = Ledger::formatKeyForAsset ( index, ASSET_TYPE );
    string typeName = this->getValue < string >( keyForAssetType );

    string keyForAssetDefinition = Ledger::formatKeyForAssetDefinition ( typeName );
    shared_ptr < AssetDefinition > assetDefinition = this->getJSONSerializableObject < AssetDefinition >( keyForAssetDefinition );
    if ( !assetDefinition ) return NULL;
    
    shared_ptr < Asset > asset = make_shared < Asset >( assetDefinition );
    asset->mType    = typeName;
    asset->mIndex   = index;
    asset->mOwner   = this->getValue < string >( keyForAssetOwner );
    
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
shared_ptr < Block > Ledger::getBlock ( size_t height ) const {

    VersionedStore snapshot ( *this );
    if ( height < snapshot.getVersion ()) {
        snapshot.revert ( height );
    }
    return Ledger::getJSONSerializableObject < Block >( snapshot, BLOCK_KEY );
}

//----------------------------------------------------------------//
Entropy Ledger::getEntropy () const {

    string entropy = this->getValueOrFallback < string >( ENTROPY, "" );
    return entropy.size () > 0 ? Entropy ( entropy ) : Entropy ();
}

//----------------------------------------------------------------//
string Ledger::getIdentity () const {

    return this->getValueOrFallback < string >( IDENTITY, "" );
}

//----------------------------------------------------------------//
SerializableList < Asset > Ledger::getInventory ( string accountName ) const {

    SerializableList < Asset > assets;

    string keyForAccountHeadIndex = this->formatKeyForAccount ( accountName, ACCOUNT_HEAD );
    Asset::Index cursor = this->getValue < Asset::Index >( keyForAccountHeadIndex );
    
    while ( cursor != Asset::NULL_ASSET_ID ) {
    
        shared_ptr < Asset > asset = this->getAsset ( cursor );
        assert ( asset );
        assets.push_back ( *asset );
    
        cursor = this->getValue < Asset::Index >( Ledger::formatKeyForAsset ( cursor, ASSET_NEXT ));
    }
    return assets;
}

//----------------------------------------------------------------//
//Inventory Ledger::getInventory ( string accountName ) const {
//
//    Inventory inventory;
//    
////    try {
////        VersionedCollectionIterator < string > collectionIt ( *this, Ledger::formatInventoryKey ( accountName ));
////        
////        for ( ; collectionIt; ++collectionIt ) {
////            BulkAssetIdentifier assetIdentifier;
////            FromJSONSerializer::fromJSONString ( assetIdentifier, *collectionIt );
////            inventory.mAssets.push_back ( assetIdentifier );
////        }
////    }
////    catch ( VersionedCollectionNotFoundException ) {
////    }
//    return inventory;
//}

//----------------------------------------------------------------//
shared_ptr < KeyInfo > Ledger::getKeyInfo ( string keyID ) const {

    return this->getJSONSerializableObject < KeyInfo >( KEY_ID + keyID );
}

//----------------------------------------------------------------//
shared_ptr < AssetMethod > Ledger::getMethod ( string methodName ) const {

    return this->getJSONSerializableObject < AssetMethod >( Ledger::formatKeyForAssetMethod ( methodName ));
}

//----------------------------------------------------------------//
shared_ptr < MinerInfo > Ledger::getMinerInfo ( string accountName ) const {

    return this->getJSONSerializableObject < MinerInfo >( prefixKey ( MINER_INFO, accountName ));
}

//----------------------------------------------------------------//
map < string, MinerInfo > Ledger::getMiners () const {

    map < string, MinerInfo > minerInfoMap;

    shared_ptr < SerializableSet < string >> miners = this->getJSONSerializableObject < SerializableSet < string >>( MINERS );
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

    return this->getJSONSerializableObject < MinerURLMap >( MINER_URLS );
}

//----------------------------------------------------------------//
shared_ptr < Schema > Ledger::getSchema ( string schemaName ) const {

    return this->getJSONSerializableObject < Schema >( Ledger::formatSchemaKey ( schemaName ));
}

//----------------------------------------------------------------//
list < Schema > Ledger::getSchemas () const {

    list < Schema > schemaList;
    const int schemaCount = this->getValue < int >( Ledger::formatKeyForSchemaCount ());
    for ( int i = 0; i < schemaCount; ++i ) {
        string name = this->getValue < string >( Ledger::formatSchemaKey ( i ));
        shared_ptr < Schema > schema = this->getJSONSerializableObject < Schema >( name );
        assert ( schema );
        schemaList.push_back ( *schema );
    }
    return schemaList;
}

//----------------------------------------------------------------//
shared_ptr < Block > Ledger::getTopBlock () const {

    return this->getJSONSerializableObject < Block >( BLOCK_KEY );
}

//----------------------------------------------------------------//
UnfinishedBlockList Ledger::getUnfinished () {

    shared_ptr < UnfinishedBlockList > unfinished = this->getJSONSerializableObject < UnfinishedBlockList >( UNFINISHED );
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

    string keyForAccount = Ledger::prefixKey ( ACCOUNT, accountName );
    if ( !this->hasValue ( keyForAccount )) return false;

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
bool Ledger::openAccount ( string sponsorName, string suffix, u64 grant, const CryptoKey& key ) {

    if ( Ledger::isChildName ( sponsorName )) return false;
    if ( !Ledger::isSuffix ( suffix )) return false;
    
    // the child name will be prepended following a tilde: "~<sponsorName>.<childSuffix>"
    // i.e. "~maker.000.000.000"
    // this prevents it from sponsoring any new accounts until it is renamed.

    string childName = Format::write ( "~%s.%s", sponsorName.c_str (), suffix.c_str ());
    assert ( Ledger::isChildName ( childName ));

    shared_ptr < Account > account = this->getAccount ( sponsorName );
    if ( account && ( account->mBalance >= grant )) {

        if ( this->getAccount ( childName )) return false;

        Account accountUpdated = *account;
        accountUpdated.mBalance -= grant;
        this->setAccount ( sponsorName, accountUpdated );
        
        Account child;
        child.mBalance = grant;
        child.mKeys [ MASTER_KEY_NAME ] = KeyAndPolicy ( key );
        this->setAccount ( childName, child );

        return true;
    }
    return false;
}

//----------------------------------------------------------------//
string Ledger::prefixKey ( string prefix, string key ) {

    return prefix + "." + key;
}

//----------------------------------------------------------------//
bool Ledger::publishSchema ( string accountName, string schemaName, const Schema& schema ) {

    // TODO: check account permissions

    schemaName = Ledger::formatSchemaKey ( schemaName );

    if ( this->hasValue ( schemaName )) return false;

    string keyForSchemaCount = Ledger::formatKeyForSchemaCount ();

    int schemaIndex = this->getValue < int >( keyForSchemaCount );
    
    string schemaKey = Ledger::formatSchemaKey ( schemaIndex );

    this->setValue < string >( schemaKey, schemaName );
    this->setJSONSerializableObject < Schema >( schemaName, schema );
    this->setValue < int >( keyForSchemaCount, schemaIndex + 1 );

    Schema::Definitions::const_iterator definitionIt = schema.mDefinitions.cbegin ();
    for ( ; definitionIt != schema.mDefinitions.cend (); ++definitionIt ) {
    
        string typeName = definitionIt->first;
        const AssetDefinition& definition = definitionIt->second;
    
        // store the definition for easy access later
        string keyForAssetDefinition = Ledger::formatKeyForAssetDefinition ( typeName );
        this->setJSONSerializableObject < AssetDefinition >( keyForAssetDefinition, definition );
    }

    Schema::Methods::const_iterator methodIt = schema.mMethods.cbegin ();
    for ( ; methodIt != schema.mMethods.cend (); ++methodIt ) {
    
        string methodName = methodIt->first;
        const AssetMethod& method = methodIt->second;
        
        // store the method for easy access later
        string keyForAssetMethod = Ledger::formatKeyForAssetMethod ( methodName );
        if ( this->hasValue ( keyForAssetMethod )) return false; // can't overwrite methods
        this->setJSONSerializableObject < AssetMethod >( keyForAssetMethod, method );
    }

    LuaContext lua ( schema.mLua );
    lua.invoke ( *this, accountName );
    
    return true;
}

//----------------------------------------------------------------//
bool Ledger::registerMiner ( string accountName, string keyName, string url ) {

    AccountKey accountKey = this->getAccountKey ( accountName, keyName );
    if ( accountKey ) {

        this->setJSONSerializableObject < MinerInfo >( prefixKey ( MINER_INFO, accountName ), MinerInfo ( accountName, url, accountKey.mKeyAndPolicy->mKey ));
        
        // TODO: find an efficient way to do all this
        shared_ptr < SerializableMap < string, string >> minerURLs = this->getJSONSerializableObject < SerializableMap < string, string >>( MINER_URLS );
        assert ( minerURLs );
        
        ( *minerURLs )[ accountName ] = url;
        this->setJSONSerializableObject < SerializableMap < string, string >>( MINER_URLS, *minerURLs );

        shared_ptr < SerializableSet < string >> miners = this->getJSONSerializableObject < SerializableSet < string >>( MINERS );
        assert ( miners );
        
        miners->insert ( accountName );
        this->setJSONSerializableObject < SerializableSet < string >>( MINERS, *miners );

        return true;
    }
    return false;
}

//----------------------------------------------------------------//
bool Ledger::renameAccount ( string accountName, string revealedName, Digest nameHash, Digest nameSecret ) {
    UNUSED ( accountName );
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
    
    return false;
}

//----------------------------------------------------------------//
void Ledger::reset () {

    this->clear ();
    this->setJSONSerializableObject < SerializableSet < string >>( MINERS, SerializableSet < string > ());
    this->setJSONSerializableObject < SerializableMap < string, string >>( MINER_URLS, SerializableMap < string, string > ());
    this->setValue < int >( Ledger::formatKeyForSchemaCount (), 0 );
    this->setValue < Asset::Index >( Ledger::formatKeyForAssetCounter (), 0 );
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

    this->setJSONSerializableObject < Account >( prefixKey ( ACCOUNT, accountName ), account );
}

//----------------------------------------------------------------//
bool Ledger::setAssetFieldValue ( Asset::Index index, string fieldName, const AssetFieldValue& field ) {

    // make sure the asset exists
    string keyForAssetType = Ledger::formatKeyForAsset ( index, ASSET_TYPE );
    if ( !this->hasValue ( keyForAssetType )) return false;

    string assetType = this->getValue < string >( keyForAssetType );

    // make sure the field exists
    string keyForAssetDefinition = Ledger::formatKeyForAssetDefinition ( assetType );
    shared_ptr < AssetDefinition > assetDefinition = this->getJSONSerializableObject < AssetDefinition >( keyForAssetDefinition );
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
    this->setJSONSerializableObject < Block >( BLOCK_KEY, block );
}

//----------------------------------------------------------------//
void Ledger::setEntropyString ( string entropy ) {

    this->setValue < string >( ENTROPY, entropy );
}

//----------------------------------------------------------------//
bool Ledger::setIdentity ( string identity ) {

    if ( this->hasValue ( IDENTITY )) return false;
    this->setValue < string >( IDENTITY, identity );
    return true;
}

//----------------------------------------------------------------//
void Ledger::setMinerInfo ( string accountName, const MinerInfo& minerInfo ) {

    this->setJSONSerializableObject < MinerInfo >( prefixKey ( ACCOUNT, accountName ), minerInfo );
}

//----------------------------------------------------------------//
void Ledger::setUnfinished ( const UnfinishedBlockList& unfinished ) {

    this->setJSONSerializableObject < UnfinishedBlockList >( UNFINISHED, unfinished );
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
