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
LedgerResult Ledger_Inventory::awardAssets ( AccountODBM& accountODBM, u64 inventoryNonce, const list < AssetBase >& assets, InventoryLogEntry& logEntry ) {

    size_t quantity = assets.size ();
    if ( quantity == 0 ) return true;

    Ledger& ledger = this->getLedger ();
    const Schema& schema = ledger.getSchema ();

    LedgerKey KEY_FOR_GLOBAL_ASSET_COUNT = Ledger::keyFor_globalAssetCount ();
    size_t globalAssetCount = ledger.getValueOrFallback < u64 >( KEY_FOR_GLOBAL_ASSET_COUNT, 0 );
    size_t accountAssetCount = accountODBM.mAssetCount.get ( 0 );
    
    list < AssetBase >::const_iterator assetIt = assets.cbegin ();
    for ( size_t i = 0; assetIt != assets.cend (); ++assetIt, ++i ) {
    
        const AssetBase& asset = *assetIt;
    
        string assetType = asset.mType;
        if ( !schema.getDefinitionOrNull ( assetType )) return Format::write ( "Asset type '%s' not found.", assetType.c_str ());
    
        AssetODBM assetODBM ( ledger, globalAssetCount );
        
        assetODBM.mOwner.set ( accountODBM.mAccountID + i );
        assetODBM.mInventoryNonce.set ( inventoryNonce );
        assetODBM.mPosition.set ( accountAssetCount + i );
        assetODBM.mType.set ( assetType );
        
        Asset::Fields::const_iterator fieldIt = asset.mFields.cbegin ();
        for ( ; fieldIt != asset.mFields.end (); ++fieldIt ) {
            assetODBM.setFieldValue ( fieldIt->first, fieldIt->second );
        }
        
        accountODBM.getInventoryField ( assetODBM.mPosition.get ()).set ( assetODBM.mAssetID );
        logEntry.insertAddition ( assetODBM.mAssetID );
    }
    
    ledger.setValue < u64 >( KEY_FOR_GLOBAL_ASSET_COUNT, globalAssetCount + quantity );
    accountODBM.mAssetCount.set ( accountAssetCount + quantity );

    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::awardAssets ( AccountODBM& accountODBM, u64 inventoryNonce, string assetType, size_t quantity, InventoryLogEntry& logEntry ) {

    if ( quantity == 0 ) return true;

    Ledger& ledger = this->getLedger ();
    const Schema& schema = ledger.getSchema ();

    if ( !schema.getDefinitionOrNull ( assetType )) return Format::write ( "Asset type '%s' not found.", assetType.c_str ());

    LedgerKey KEY_FOR_GLOBAL_ASSET_COUNT = Ledger::keyFor_globalAssetCount ();
    size_t globalAssetCount = ledger.getValueOrFallback < u64 >( KEY_FOR_GLOBAL_ASSET_COUNT, 0 );
    size_t accountAssetCount = accountODBM.mAssetCount.get ( 0 );
    
    for ( size_t i = 0; i < quantity; ++i ) {
        
        AssetODBM assetODBM ( ledger, globalAssetCount + i );
                
        assetODBM.mOwner.set ( accountODBM.mAccountID );
        assetODBM.mInventoryNonce.set ( inventoryNonce );
        assetODBM.mPosition.set ( accountAssetCount + i );
        assetODBM.mType.set ( assetType );
        
        accountODBM.getInventoryField ( assetODBM.mPosition.get ()).set ( assetODBM.mAssetID );
        logEntry.insertAddition ( assetODBM.mAssetID );
    }
    
    ledger.setValue < u64 >( KEY_FOR_GLOBAL_ASSET_COUNT, globalAssetCount + quantity );
    accountODBM.mAssetCount.set ( accountAssetCount + quantity );

    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::awardAssets ( AccountID accountID, string assetType, size_t quantity, time_t time ) {

    Ledger& ledger = this->getLedger ();
    
    AccountODBM accountODBM ( ledger, accountID );
    if ( accountODBM.mAccountID == AccountID::NULL_INDEX ) return "Account not found.";
    u64 inventoryNonce = accountODBM.mInventoryNonce.get ( 0 );

    InventoryLogEntry logEntry ( time );
    this->awardAssets ( accountODBM, inventoryNonce, assetType, quantity, logEntry );
   
    ledger.updateInventory ( accountODBM.mAccountID, logEntry );

    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::awardAssets ( AccountID accountID, const list < AssetBase >& assets, time_t time ) {

    Ledger& ledger = this->getLedger ();
    
    AccountODBM accountODBM ( ledger, accountID );
    if ( accountODBM.mAccountID == AccountID::NULL_INDEX ) return "Account not found.";
    u64 inventoryNonce = accountODBM.mInventoryNonce.get ( 0 );

    InventoryLogEntry logEntry ( time );
    this->awardAssets ( accountODBM, inventoryNonce, assets, logEntry );
   
    ledger.updateInventory ( accountODBM.mAccountID, logEntry );

    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::awardAssetsAll ( AccountID accountID, size_t quantity, time_t time ) {

    Ledger& ledger = this->getLedger ();
    const Schema& schema = ledger.getSchema ();
    
    AccountODBM accountODBM ( ledger, accountID );
    if ( accountODBM.mAccountID == AccountID::NULL_INDEX ) return "Account not found.";
    u64 inventoryNonce = accountODBM.mInventoryNonce.get ( 0 );
    
    InventoryLogEntry logEntry ( time );
    
    const Schema::Definitions& definitions = schema.getDefinitions ();
    Schema::Definitions::const_iterator definitionIt = definitions.cbegin ();
    for ( ; definitionIt != definitions.cend (); ++definitionIt ) {
        ledger.awardAssets ( accountODBM, inventoryNonce, definitionIt->first, quantity, logEntry );
    }
    
    ledger.updateInventory ( accountODBM.mAccountID, logEntry );
    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::awardAssetsRandom ( AccountID accountID, string deckName, string seed, size_t quantity, time_t time ) {

    Ledger& ledger = this->getLedger ();
    const Schema& schema = ledger.getSchema ();

    if ( quantity == 0 ) return true;

    const Schema::Deck* deck = schema.getDeck ( deckName );
    if ( !deck ) return Format::write ( "Deck '%s' not found.", deckName.c_str ());

    AccountODBM accountODBM ( ledger, accountID );
    if ( accountODBM.mAccountID == AccountID::NULL_INDEX ) return false;

    // TODO: yes, this is inefficient. optimize (and/or cache) later.
    vector < string > expandedSetOrDeck;
    Schema::Deck::const_iterator deckIt = deck->cbegin ();
    for ( ; deckIt != deck->cend (); ++deckIt ) {
        string assetType = deckIt->first;
        size_t count = deckIt->second;
        for ( size_t i = 0; i < count; ++i ) {
            expandedSetOrDeck.push_back ( assetType );
        }
    }
    
    string entropy = ledger.getEntropyString ();
    
    Poco::Crypto::DigestEngine digestEngine ( "SHA256" );
    
    Poco::DigestOutputStream digestStream ( digestEngine );
    digestStream << entropy;
    digestStream << deckName;
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
    
    InventoryLogEntry logEntry ( time );
    u64 inventoryNonce = accountODBM.mInventoryNonce.get ( 0 );
    
    map < string, size_t >::const_iterator awardIt = awards.cbegin ();
    for ( ; awardIt != awards.cend (); ++awardIt ) {
        ledger.awardAssets ( accountODBM, inventoryNonce, awardIt->first, awardIt->second, logEntry );
    }
    ledger.updateInventory ( accountODBM.mAccountID, logEntry );
    
    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::awardDeck ( AccountID accountID, string deckName, time_t time ) {

    Ledger& ledger = this->getLedger ();
    const Schema& schema = ledger.getSchema ();

    const Schema::Deck* deck = schema.getDeck ( deckName );
    if ( !deck ) return Format::write ( "Deck '%s' not found.", deckName.c_str ());
    
    Schema::Deck::const_iterator deckIt = deck->cbegin ();
    for ( ; deckIt != deck->cend (); ++deckIt ) {
        ledger.awardAssets ( accountID, deckIt->first, deckIt->second, time );
    }
    return true;
}

//----------------------------------------------------------------//
AssetID::Index Ledger_Inventory::getAssetID ( string assetID ) const {

    const Ledger& ledger = this->getLedger ();

    AssetID::Index assetIndex = AssetID::decode ( assetID );
    
    AssetODBM assetODBM ( ledger, assetIndex );
    if ( assetODBM.mOwner.get () == AccountID::NULL_INDEX ) return AssetID::NULL_INDEX; // if an asset doesn't have an owner, it doesn't exist.
    
    return assetIndex;
}

//----------------------------------------------------------------//
void Ledger_Inventory::getInventory ( AccountID accountID, SerializableList < SerializableSharedConstPtr < Asset >>& assetList, size_t max, bool sparse ) {

    Ledger& ledger = this->getLedger ();

    SerializableList < Asset > assets;

    AccountODBM accountODBM ( ledger, accountID );
    if ( accountODBM.mAccountID == AccountID::NULL_INDEX ) return;

    size_t assetCount = accountODBM.mAssetCount.get ( 0 );
    
    if (( max > 0 ) && ( max < assetCount )) {
        assetCount = max;
    }
    
    for ( size_t i = 0; i < assetCount; ++i ) {
    
        AssetID::Index assetIndex = accountODBM.getInventoryField ( i ).get ();
        shared_ptr < const Asset > asset = AssetODBM ( ledger, assetIndex ).getAsset ( sparse );
        assert ( asset );
        assetList.push_back ( asset );
    }
}

//----------------------------------------------------------------//
map < string, size_t > Ledger_Inventory::getInventoryHistogram ( AccountID accountID ) {

    map < string, size_t > histogram;

    SerializableList < SerializableSharedConstPtr < Asset >> inventory;
    this->getInventory ( accountID, inventory );

    SerializableList < SerializableSharedConstPtr < Asset >>::const_iterator inventoryIt = inventory.cbegin ();
    for ( ; inventoryIt != inventory.cend (); ++inventoryIt ) {
        SerializableSharedConstPtr < Asset > asset = *inventoryIt;

        if ( histogram.find ( asset->mType ) == histogram.cend ()) {
            histogram [ asset->mType ] = 0;
        }
        histogram [ asset->mType ] = histogram [ asset->mType ] + 1;
    }
    return histogram;
}

//----------------------------------------------------------------//
bool Ledger_Inventory::resetAssetFieldValue ( AssetID::Index index, string fieldName, time_t time ) {

    Ledger& ledger = this->getLedger ();
    const Schema& schema = ledger.getSchema ();

    AssetODBM assetODBM ( ledger, index );
    if ( assetODBM.mAssetID == AssetID::NULL_INDEX ) return false;
    
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
    
    this->updateInventory ( assetODBM, time, InventoryLogEntry::EntryOp::UPDATE_ASSET );
    return true;
}

//----------------------------------------------------------------//
bool Ledger_Inventory::revokeAsset ( AssetID::Index index, time_t time ) {

    Ledger& ledger = this->getLedger ();

    // make sure the asset exists
    AssetODBM assetODBM ( ledger, index );
    if ( assetODBM.mAssetID == AssetID::NULL_INDEX ) return false;

    // get the owner
    AccountID accountID = assetODBM.mOwner.get ();
    if ( accountID == AccountID::NULL_INDEX ) return true; // already revoked!
    AccountODBM accountODBM ( ledger, accountID );

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
        accountInventoryField.set ( tailAssetODBM.mAssetID );
    }
    
    // asset has no owner or position
    assetODBM.mOwner.set ( AssetID::NULL_INDEX );
    assetODBM.mInventoryNonce.set (( u64 )-1 );
    assetODBM.mPosition.set ( Asset::NULL_POSITION );
    
    // shrink account inventory by one
    accountODBM.mAssetCount.set ( accountAssetCount - 1 );
    
    InventoryLogEntry logEntry ( time );
    logEntry.insertDeletion ( assetODBM.mAssetID );
    ledger.updateInventory ( accountODBM.mAccountID, logEntry );
    
    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::setAssetFieldValue ( AssetID::Index index, string fieldName, const AssetFieldValue& field, time_t time ) {

    AssetODBM assetODBM ( this->getLedger (), index );
    if ( assetODBM.mAssetID == AssetID::NULL_INDEX ) return "No such account.";
    
    LedgerResult result = assetODBM.setFieldValue ( fieldName, field );
    if ( !result ) return result;

    this->updateInventory ( assetODBM, time, InventoryLogEntry::EntryOp::UPDATE_ASSET );
    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::transferAssets ( AccountID senderAccountIndex, AccountID receiverAccountIndex, const string* assetIdentifiers, size_t totalAssets, time_t time ) {
    
    Ledger& ledger = this->getLedger ();
    
    AccountODBM senderODBM ( ledger, senderAccountIndex );
    AccountODBM receiverODBM ( ledger, receiverAccountIndex );

    if ( senderODBM.mAccountID == AccountID::NULL_INDEX ) return "Count not find sender account.";
    if ( receiverODBM.mAccountID == AccountID::NULL_INDEX ) return "Could not find recipient account.";
    if ( senderODBM.mAccountID == receiverODBM.mAccountID ) return "Cannot send assets to self.";

    size_t senderAssetCount = senderODBM.mAssetCount.get ( 0 );
    size_t receiverAssetCount = receiverODBM.mAssetCount.get ( 0 );

    shared_ptr < const Account > receiverAccount = receiverODBM.mBody.get ();
    Entitlements receiverEntitlements = ledger.getEntitlements < AccountEntitlements >( *receiverAccount );
    if ( !receiverEntitlements.check ( AccountEntitlements::MAX_ASSETS, receiverAssetCount + totalAssets )) {
        double max = receiverEntitlements.resolvePathAs < NumericEntitlement >( AccountEntitlements::MAX_ASSETS )->getUpperLimit ().mLimit;
        return Format::write ( "Transaction would overflow receiving account's inventory limit of %d assets.", ( int )max );
    }

    // check all the assets
    for ( size_t i = 0; i < totalAssets; ++i ) {
        string assetIdentifier ( assetIdentifiers [ i ]);
        AssetODBM assetODBM ( ledger, AssetID::decode ( assetIdentifiers [ i ]));
        if ( assetODBM.mAssetID == AssetID::NULL_INDEX ) return Format::write ( "Count not find asset %s.", assetIdentifier.c_str ());
        if ( assetODBM.mOwner.get () != senderODBM.mAccountID ) return Format::write ( "Asset %s is not owned by %s.", assetIdentifier.c_str (), senderODBM.mName.get ().c_str ());
    }
    
    InventoryLogEntry senderLogEntry ( time );
    InventoryLogEntry receiverLogEntry ( time );

    for ( size_t i = 0; i < totalAssets; ++i, --senderAssetCount, ++receiverAssetCount ) {
        
        AssetODBM assetODBM ( ledger, AssetID::decode ( assetIdentifiers [ i ]));
        
        // fill the asset's original position by swapping in the tail
        size_t position = assetODBM.mPosition.get ();
        if ( position < senderAssetCount ) {
            LedgerFieldODBM < AssetID::Index > senderInventoryField = senderODBM.getInventoryField ( position );
            LedgerFieldODBM < AssetID::Index > senderInventoryTailField = senderODBM.getInventoryField ( senderAssetCount - 1 );
            
            AssetODBM tailAssetODBM ( ledger, senderInventoryTailField.get ());
            tailAssetODBM.mPosition.set ( position );
            senderInventoryField.set ( tailAssetODBM.mAssetID );
        }
        
        // transfer asset ownership to the receiver
        assetODBM.mOwner.set ( receiverODBM.mAccountID );
        assetODBM.mInventoryNonce.set ( receiverODBM.mInventoryNonce.get ( 0 ));
        assetODBM.mPosition.set ( receiverAssetCount );
        receiverODBM.getInventoryField ( assetODBM.mPosition.get ()).set ( assetODBM.mAssetID );
        
        // add it to the log entries
        senderLogEntry.insertDeletion ( assetODBM.mAssetID );
        receiverLogEntry.insertAddition ( assetODBM.mAssetID );
    }
    
    ledger.updateInventory ( senderODBM.mAccountID, senderLogEntry );
    senderODBM.mAssetCount.set ( senderAssetCount );
    
    ledger.updateInventory ( receiverODBM.mAccountID, receiverLogEntry );
    receiverODBM.mAssetCount.set ( receiverAssetCount );
    
    return true;
}

//----------------------------------------------------------------//
void Ledger_Inventory::updateInventory ( AccountID accountID, const InventoryLogEntry& entry ) {

    Ledger& ledger = this->getLedger ();
    AccountODBM accountODBM ( ledger, accountID );

    u64 inventoryNonce = accountODBM.mInventoryNonce.get ( 0 );
    accountODBM.getInventoryLogEntryField ( inventoryNonce ).set ( entry );
    accountODBM.mInventoryNonce.set ( inventoryNonce + 1 );
}

//----------------------------------------------------------------//
void Ledger_Inventory::updateInventory ( AssetODBM& assetODBM, time_t time, InventoryLogEntry::EntryOp op ) {

    Ledger& ledger = this->getLedger ();

    AccountID ownerIndex = assetODBM.mOwner.get ( AccountID::NULL_INDEX );
    if ( ownerIndex != AccountID::NULL_INDEX  ) {
    
        AccountODBM accountODBM ( ledger, ownerIndex );
        assetODBM.mInventoryNonce.set ( accountODBM.mInventoryNonce.get ( 0 ));
        
        InventoryLogEntry logEntry ( time );
        logEntry.insert ( assetODBM.mAssetID, op );
        ledger.updateInventory ( ownerIndex, logEntry );
    }
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::upgradeAssets ( AccountID accountID, const map < string, string >& upgrades, time_t time ) {

    Ledger& ledger = this->getLedger ();
    const Schema& schema = ledger.getSchema ();
    
    AccountODBM accountODBM ( ledger, accountID );
    if ( accountODBM.mAccountID == AccountID::NULL_INDEX ) return "No such account.";

    // check the upgrades
    SerializableMap < string, string >::const_iterator upgradeIt = upgrades.cbegin ();
    for ( ; upgradeIt != upgrades.end (); ++upgradeIt ) {
        
        string assetID = upgradeIt->first;
        string upgradeType = upgradeIt->second;
        
        AssetODBM assetODBM ( ledger, AssetID::decode ( assetID ));

        if ( !assetODBM.mOwner.exists ()) return Format::write ( "Asset %s does not exist.", assetID.c_str ());
        if ( assetODBM.mOwner.get () != accountODBM.mAccountID ) return Format::write ( "Asset %s does not belong to account %s.", assetID.c_str (), accountODBM.mName.get ().c_str ());
        if ( !schema.canUpgrade ( assetODBM.mType.get (), upgradeType )) return Format::write (  "Cannot upgrade asset %s to %s.",  assetID.c_str (),  upgradeType.c_str ());
    }
    
    u64 inventoryNonce = accountODBM.mInventoryNonce.get ( 0 );
    InventoryLogEntry logEntry ( time );
    
    // perform the upgrades
    upgradeIt = upgrades.cbegin ();
    for ( ; upgradeIt != upgrades.end (); ++upgradeIt ) {
        
        AssetODBM assetODBM ( ledger, AssetID::decode ( upgradeIt->first ) );
        assetODBM.mType.set ( upgradeIt->second );
        assetODBM.mInventoryNonce.set ( inventoryNonce );
        
        logEntry.insertDeletion ( assetODBM.mAssetID );
        logEntry.insertAddition ( assetODBM.mAssetID );
    }
    ledger.updateInventory ( accountODBM.mAccountID, logEntry );
    return true;
}

} // namespace Volition
