// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AccountODBM.h>
#include <volition/Asset.h>
#include <volition/AssetMethod.h>
#include <volition/AssetMethodInvocation.h>
#include <volition/AssetODBM.h>
#include <volition/Block.h>
#include <volition/Format.h>
#include <volition/InventoryLogEntry.h>
#include <volition/Ledger.h>
#include <volition/Ledger_Inventory.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/LuaContext.h>
#include <volition/TransactionMaker.h>

namespace Volition {

//================================================================//
// Ledger_Inventory
//================================================================//

//----------------------------------------------------------------//
bool Ledger_Inventory::awardAssets ( const Schema& schema, AccountODBM& accountODBM, u64 inventoryNonce, string assetType, size_t quantity, InventoryLogEntry& logEntry ) {

    Ledger& ledger = this->getLedger ();

    if ( quantity == 0 ) return true;
    if ( !schema.getDefinitionOrNull ( assetType )) return false;

    LedgerKey KEY_FOR_GLOBAL_ASSET_COUNT = Ledger::keyFor_globalAssetCount ();
    size_t globalAssetCount = ledger.getValueOrFallback < u64 >( KEY_FOR_GLOBAL_ASSET_COUNT, 0 );
    size_t accountAssetCount = accountODBM.mAssetCount.get ( 0 );
    
    for ( size_t i = 0; i < quantity; ++i ) {
        
        AssetODBM assetODBM ( ledger, globalAssetCount + i );
                
        assetODBM.mOwner.set ( accountODBM.mIndex );
        assetODBM.mInventoryNonce.set ( inventoryNonce );
        assetODBM.mPosition.set ( accountAssetCount + i );
        assetODBM.mType.set ( assetType );
        
        accountODBM.getInventoryField ( assetODBM.mPosition.get ()).set ( assetODBM.mIndex );
        logEntry.insertAddition ( assetODBM.mIndex );
    }
    
    ledger.setValue < u64 >( KEY_FOR_GLOBAL_ASSET_COUNT, globalAssetCount + quantity );
    accountODBM.mAssetCount.set ( accountAssetCount + quantity );

    return true;
}

//----------------------------------------------------------------//
bool Ledger_Inventory::awardAssets ( const Schema& schema, Account::Index accountIndex, string assetType, size_t quantity ) {

    Ledger& ledger = this->getLedger ();
    
    AccountODBM accountODBM ( ledger, accountIndex );
    if ( accountODBM.mIndex == Account::NULL_INDEX ) return false;
    u64 inventoryNonce = accountODBM.mInventoryNonce.get ( 0 );

    InventoryLogEntry logEntry;
    this->awardAssets ( schema, accountODBM, inventoryNonce, assetType, quantity, logEntry );
   
    ledger.setInventoryLogEntry ( accountODBM.mIndex, inventoryNonce, logEntry );
    accountODBM.mInventoryNonce.set ( inventoryNonce + 1 );

    return true;
}

//----------------------------------------------------------------//
bool Ledger_Inventory::awardAssetsRandom ( const Schema& schema, Account::Index accountIndex, string setOrDeckName, string seed, size_t quantity ) {

    Ledger& ledger = this->getLedger ();

    if ( quantity == 0 ) return 0;

    const Schema::Deck* setOrDeck = schema.getSetOrDeck ( setOrDeckName );
    if ( !setOrDeck ) return 0;

    AccountODBM accountODBM ( ledger, accountIndex );
    if ( accountODBM.mIndex == Account::NULL_INDEX ) return false;

    // TODO: yes, this is inefficient. optimize (and/or cache) later.
    vector < string > expandedSetOrDeck;
    Schema::Deck::const_iterator setOrDeckIt = setOrDeck->cbegin ();
    for ( ; setOrDeckIt != setOrDeck->cend (); ++setOrDeckIt ) {
        string assetType = setOrDeckIt->first;
        size_t count = setOrDeckIt->second;
        for ( size_t i = 0; i < count; ++i ) {
            expandedSetOrDeck.push_back ( assetType );
        }
    }
    
    string entropy = ledger.getEntropyString ();
    
    Poco::Crypto::DigestEngine digestEngine ( "SHA256" );
    
    Poco::DigestOutputStream digestStream ( digestEngine );
    digestStream << entropy;
    digestStream << setOrDeckName;
    digestStream << seed;
    digestStream.close ();

    Poco::DigestEngine::Digest digest = digestEngine.digest ();
    assert ( digest.size () == 32 );
    const u32* seedVals = ( const u32* )digest.data ();

    std::mt19937 gen;
    std::seed_seq sseq {
        seedVals [ 0 ],
        seedVals [ 1 ],
        seedVals [ 2 ],
        seedVals [ 3 ],
        seedVals [ 4 ],
        seedVals [ 5 ],
        seedVals [ 6 ],
        seedVals [ 7 ],
    };
    gen.seed ( sseq );
    
    map < string, size_t > awards;
    for ( size_t i = 0; i < quantity; ++i ) {
        u32 index = gen ();
        string awardType = expandedSetOrDeck [ index % expandedSetOrDeck.size ()];
        
        if ( awards.find ( awardType ) == awards.end ()) {
            awards [ awardType ] = 0;
        }
        awards [ awardType ] = awards [ awardType ] + 1;
    }
    
    InventoryLogEntry logEntry;
    u64 inventoryNonce = accountODBM.mInventoryNonce.get ( 0 );
    
    map < string, size_t >::const_iterator awardIt = awards.cbegin ();
    for ( ; awardIt != awards.cend (); ++awardIt ) {
        ledger.awardAssets ( schema, accountODBM, inventoryNonce, awardIt->first, awardIt->second, logEntry );
    }
    ledger.setInventoryLogEntry ( accountODBM.mIndex, inventoryNonce, logEntry );
    accountODBM.mInventoryNonce.set ( inventoryNonce + 1 );
    
    return true;
}

//----------------------------------------------------------------//
shared_ptr < Asset > Ledger_Inventory::getAsset ( const Schema& schema, AssetID::Index index ) const {

    const Ledger& ledger = this->getLedger ();

    AssetODBM assetODBM ( ledger, index );
    if ( !assetODBM.mOwner.exists ()) return NULL;

    const AssetDefinition* assetDefinition = schema.getDefinitionOrNull ( assetODBM.mType.get ());
    if ( !assetDefinition ) return NULL;
    
    shared_ptr < Asset > asset = make_shared < Asset >();
    asset->mType            = assetODBM.mType.get ();
    asset->mAssetID         = assetODBM.mIndex;
    asset->mOwner           = ledger.getAccountName ( assetODBM.mOwner.get ());
    asset->mInventoryNonce  = assetODBM.mInventoryNonce.get ( 0 );
    
    // copy the fields and apply any overrides
    AssetDefinition::Fields::const_iterator fieldIt = assetDefinition->mFields.cbegin ();
    for ( ; fieldIt != assetDefinition->mFields.cend (); ++fieldIt ) {
        
        string fieldName = fieldIt->first;
        
        const AssetFieldDefinition& field = fieldIt->second;
        AssetFieldValue value = field;
        
        if ( field.mMutable ) {

            LedgerKey KEY_FOR_ASSET_FIELD = AssetODBM::keyFor_field ( index, fieldName );

            switch ( field.getType ()) {
            
                case AssetFieldValue::Type::TYPE_BOOL:
                    value = ledger.getValueOrFallback < bool >( KEY_FOR_ASSET_FIELD, value.strictBoolean ());
                    break;
                    
                case AssetFieldValue::Type::TYPE_NUMBER:
                    value = ledger.getValueOrFallback < double >( KEY_FOR_ASSET_FIELD, value.strictNumber ());
                    break;
                    
                case AssetFieldValue::Type::TYPE_STRING:
                    value = ledger.getValueOrFallback < string >( KEY_FOR_ASSET_FIELD, value.strictString ());
                    break;
                    
                default:
                    break;
            }
        }
        asset->mFields [ fieldName ] = value;
    }
    return asset;
}

//----------------------------------------------------------------//
void Ledger_Inventory::getInventory ( const Schema& schema, Account::Index accountIndex, SerializableList < SerializableSharedPtr < Asset >>& assetList, size_t max ) const {

    const Ledger& ledger = this->getLedger ();

    SerializableList < Asset > assets;

    AccountODBM accountODBM ( ledger, accountIndex );
    if ( accountODBM.mIndex == Account::NULL_INDEX ) return;

    size_t assetCount = accountODBM.mAssetCount.get ();
    
    if (( max > 0 ) && ( max < assetCount )) {
        assetCount = max;
    }
    
    for ( size_t i = 0; i < assetCount; ++i ) {
    
        AssetID::Index assetIndex = accountODBM.getInventoryField ( i ).get ();
        shared_ptr < Asset > asset = ledger.getAsset ( schema, assetIndex );
        assert ( asset );
        assetList.push_back ( asset );
    }
}

//----------------------------------------------------------------//
shared_ptr < InventoryLogEntry > Ledger_Inventory::getInventoryLogEntry ( Account::Index accountIndex, u64 inventoryNonce ) const {

    return this->getLedger ().getObjectOrNull < InventoryLogEntry >( AccountODBM::keyFor_inventoryLogEntry ( accountIndex, inventoryNonce ));
}

//----------------------------------------------------------------//
bool Ledger_Inventory::resetAssetFieldValue ( const Schema& schema, AssetID::Index index, string fieldName ) {

    Ledger& ledger = this->getLedger ();

    AssetODBM assetODBM ( ledger, index );
    if ( assetODBM.mIndex == AssetID::NULL_INDEX ) return false;
    
    string assetType = assetODBM.mType.get ();
    const AssetDefinition* assetDefinition = schema.getDefinitionOrNull ( assetType );
    if ( !assetDefinition ) return false;

    AssetFieldDefinition fieldDefinition = assetDefinition->getField ( fieldName );
    if ( !fieldDefinition.mMutable ) return false;

    LedgerKey KEY_FOR_ASSET_FIELD = AssetODBM::keyFor_field ( index, fieldName );

    if ( ledger.hasKey ( KEY_FOR_ASSET_FIELD )) {

        switch ( fieldDefinition.getType ()) {
        
            case AssetFieldValue::Type::TYPE_BOOL:
                ledger.setValue < bool >( KEY_FOR_ASSET_FIELD, fieldDefinition.strictBoolean ());
                break;
                
            case AssetFieldValue::Type::TYPE_NUMBER:
                ledger.setValue < double >( KEY_FOR_ASSET_FIELD, fieldDefinition.strictNumber ());
                break;
                
            case AssetFieldValue::Type::TYPE_STRING:
                ledger.setValue < string >( KEY_FOR_ASSET_FIELD, fieldDefinition.strictString ());
                break;
                
            default:
                return false;
        }
    }
    ledger.incAccountInventoryNonce ( assetODBM.mOwner.get ());
    return true;
}

//----------------------------------------------------------------//
bool Ledger_Inventory::revokeAsset ( Account::Index accountIndex, AssetID::Index index ) {

    Ledger& ledger = this->getLedger ();

    // make sure the account exists
    AccountODBM accountODBM ( ledger, accountIndex );
    if ( accountODBM.mIndex == Account::NULL_INDEX ) return false;

    // make sure the asset exists
    AssetODBM assetODBM ( ledger, index );
    if ( assetODBM.mIndex == AssetID::NULL_INDEX ) return false;

    // make sure the account owns the asset
    if ( assetODBM.mOwner.get () != accountODBM.mIndex ) return false;

    // count the assets in the account
    size_t accountAssetCount = accountODBM.mAssetCount.get ( 0 );
    assert ( accountAssetCount > 0 );

    // fill the asset's original position by swapping in the tail
    size_t position = assetODBM.mPosition.get ();
    if ( position < accountAssetCount ) {
        LedgerFieldODBM < AssetID::Index > accountInventoryField = accountODBM.getInventoryField ( position );
        LedgerFieldODBM < AssetID::Index > accountInventoryTailField = accountODBM.getInventoryField ( accountAssetCount - 1 );
        
        AssetODBM tailAssetODBM ( ledger, accountInventoryTailField.get ());
        tailAssetODBM.mPosition.set ( position );
        accountInventoryField.set ( tailAssetODBM.mIndex );
    }
    
    // asset has no owner or position
    assetODBM.mOwner.set ( AssetID::NULL_INDEX );
    assetODBM.mInventoryNonce.set (( u64 )-1 );
    assetODBM.mPosition.set ( Asset::NULL_POSITION );
    
    // shrink account inventory by one
    accountODBM.mAssetCount.set ( accountAssetCount - 1 );
    
    u64 inventoryNonce = accountODBM.mInventoryNonce.get ( 0 );
    InventoryLogEntry logEntry;
    logEntry.insertDeletion ( assetODBM.mIndex );
    ledger.setInventoryLogEntry ( accountODBM.mIndex, inventoryNonce, logEntry );
    accountODBM.mInventoryNonce.set ( inventoryNonce + 1 );
    
    return true;
}

//----------------------------------------------------------------//
bool Ledger_Inventory::setAssetFieldValue ( const Schema& schema, AssetID::Index index, string fieldName, const AssetFieldValue& field ) {

    Ledger& ledger = this->getLedger ();

    AssetODBM assetODBM ( ledger, index );
    if ( assetODBM.mIndex == AssetID::NULL_INDEX ) return false;
    
    string assetType = assetODBM.mType.get ();
    const AssetDefinition* assetDefinition = schema.getDefinitionOrNull ( assetType );
    if ( !assetDefinition ) return false;

    AssetFieldDefinition fieldDefinition = assetDefinition->getField ( fieldName );
    if ( !fieldDefinition.mMutable ) return false;

    LedgerKey KEY_FOR_ASSET_FIELD = AssetODBM::keyFor_field ( index, fieldName );

    switch ( field.getType ()) {
    
        case AssetFieldValue::Type::TYPE_BOOL:
            ledger.setValue < bool >( KEY_FOR_ASSET_FIELD, field.strictBoolean ());
            break;
            
        case AssetFieldValue::Type::TYPE_NUMBER:
            ledger.setValue < double >( KEY_FOR_ASSET_FIELD, field.strictNumber ());
            break;
            
        case AssetFieldValue::Type::TYPE_STRING:
            ledger.setValue < string >( KEY_FOR_ASSET_FIELD, field.strictString ());
            break;
            
        default:
            return false;
    }
    ledger.incAccountInventoryNonce ( assetODBM.mOwner.get ());
    return true;
}

//----------------------------------------------------------------//
void Ledger_Inventory::setInventoryLogEntry ( Account::Index accountIndex, u64 inventoryNonce, const InventoryLogEntry& entry ) {

    this->getLedger ().setObject < InventoryLogEntry >( AccountODBM::keyFor_inventoryLogEntry ( accountIndex, inventoryNonce ), entry );
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::transferAssets ( Account::Index senderAccountIndex, Account::Index receiverAccountIndex, const string* assetIdentifiers, size_t totalAssets ) {
    
    Ledger& ledger = this->getLedger ();
    
    AccountODBM senderODBM ( ledger, senderAccountIndex );
    AccountODBM receiverODBM ( ledger, receiverAccountIndex );

    if ( senderODBM.mIndex == Account::NULL_INDEX ) return "Count not find sender account.";
    if ( receiverODBM.mIndex == Account::NULL_INDEX ) return "Could not find recipient account.";
    if ( senderODBM.mIndex == receiverODBM.mIndex ) return "Cannot send assets to self.";

    size_t senderAssetCount = senderODBM.mAssetCount.get ( 0 );
    size_t receiverAssetCount = receiverODBM.mAssetCount.get ( 0 );

    shared_ptr < Account > receiverAccount = ledger.getAccount ( receiverODBM.mIndex );
    Entitlements receiverEntitlements = ledger.getEntitlements < AccountEntitlements >( *receiverAccount );
    if ( !receiverEntitlements.check ( AccountEntitlements::MAX_ASSETS, receiverAssetCount + totalAssets )) {
        double max = receiverEntitlements.resolvePathAs < NumericEntitlement >( AccountEntitlements::MAX_ASSETS )->getUpperLimit ().mLimit;
        return Format::write ( "Transaction would overflow receiving account's inventory limit of %d assets.", ( int )max );
    }

    // check all the assets
    for ( size_t i = 0; i < totalAssets; ++i ) {
        string assetIdentifier ( assetIdentifiers [ i ]);
        AssetODBM assetODBM ( ledger, AssetID::decode ( assetIdentifiers [ i ]));
        if ( assetODBM.mIndex == AssetID::NULL_INDEX ) return Format::write ( "Count not find asset %s.", assetIdentifier.c_str ());
        if ( assetODBM.mOwner.get () != senderODBM.mIndex ) return Format::write ( "Asset %s is not owned by %s.", assetIdentifier.c_str (), ledger.getAccountName ( senderAccountIndex ).c_str ());
    }
    
    u64 senderInventoryNonce = senderODBM.mInventoryNonce.get ( 0 );
    InventoryLogEntry senderLogEntry;
    
    u64 receiverInventoryNonce = receiverODBM.mInventoryNonce.get ( 0 );
    InventoryLogEntry receiverLogEntry;

    for ( size_t i = 0; i < totalAssets; ++i, --senderAssetCount, ++receiverAssetCount ) {
        
        AssetODBM assetODBM ( ledger, AssetID::decode ( assetIdentifiers [ i ]));
        
        // fill the asset's original position by swapping in the tail
        size_t position = assetODBM.mPosition.get ();
        if ( position < senderAssetCount ) {
            LedgerFieldODBM < AssetID::Index > senderInventoryField = senderODBM.getInventoryField ( position );
            LedgerFieldODBM < AssetID::Index > senderInventoryTailField = senderODBM.getInventoryField ( senderAssetCount - 1 );
            
            AssetODBM tailAssetODBM ( ledger, senderInventoryTailField.get ());
            tailAssetODBM.mPosition.set ( position );
            senderInventoryField.set ( tailAssetODBM.mIndex );
        }
        
        // transfer asset ownership to the receiver
        assetODBM.mOwner.set ( receiverODBM.mIndex );
        assetODBM.mInventoryNonce.set ( receiverInventoryNonce );
        assetODBM.mPosition.set ( receiverAssetCount );
        receiverODBM.getInventoryField ( assetODBM.mPosition.get ()).set ( assetODBM.mIndex );
        
        // add it to the log entries
        senderLogEntry.insertDeletion ( assetODBM.mIndex );
        receiverLogEntry.insertAddition ( assetODBM.mIndex );
    }
    
    ledger.setInventoryLogEntry ( senderODBM.mIndex, senderInventoryNonce, senderLogEntry );
    senderODBM.mAssetCount.set ( senderAssetCount );
    senderODBM.mInventoryNonce.set ( senderInventoryNonce + 1 );
    
    ledger.setInventoryLogEntry ( receiverODBM.mIndex, receiverInventoryNonce, receiverLogEntry );
    receiverODBM.mAssetCount.set ( receiverAssetCount );
    receiverODBM.mInventoryNonce.set ( receiverInventoryNonce + 1 );
    
    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::upgradeAssets ( const Schema& schema, Account::Index accountIndex, const map < string, string >& upgrades ) {

    Ledger& ledger = this->getLedger ();
    
    AccountODBM accountODBM ( ledger, accountIndex );
    if ( accountODBM.mIndex == Account::NULL_INDEX ) return "No such account.";

    // check the upgrades
    SerializableMap < string, string >::const_iterator upgradeIt = upgrades.cbegin ();
    for ( ; upgradeIt != upgrades.end (); ++upgradeIt ) {
        
        string assetID = upgradeIt->first;
        string upgradeType = upgradeIt->second;
        
        AssetODBM assetODBM ( ledger, AssetID::decode ( assetID ));

        if ( !assetODBM.mOwner.exists ()) return Format::write ( "Asset %s does not exist.", assetID.c_str ());
        if ( assetODBM.mOwner.get () != accountODBM.mIndex ) return Format::write ( "Asset %s does not belong to account %s.", assetID.c_str (), ledger.getAccountName ( accountIndex ).c_str ());
        if ( !schema.canUpgrade ( assetODBM.mType.get (), upgradeType )) return Format::write (  "Cannot upgrade asset %s to %s.",  assetID.c_str (),  upgradeType.c_str ());
    }
    
    u64 inventoryNonce = accountODBM.mInventoryNonce.get ( 0 );
    InventoryLogEntry logEntry;
    
    // perform the upgrades
    upgradeIt = upgrades.cbegin ();
    for ( ; upgradeIt != upgrades.end (); ++upgradeIt ) {
        
        AssetODBM assetODBM ( ledger, AssetID::decode ( upgradeIt->first ) );
        assetODBM.mType.set ( upgradeIt->second );
        assetODBM.mInventoryNonce.set ( inventoryNonce );
        
        logEntry.insertDeletion ( assetODBM.mIndex );
        logEntry.insertAddition ( assetODBM.mIndex );
    }
    ledger.setInventoryLogEntry ( accountODBM.mIndex, inventoryNonce, logEntry );
    accountODBM.mInventoryNonce.set ( inventoryNonce + 1 );
    return true;
}

} // namespace Volition
