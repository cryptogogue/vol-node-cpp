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
#include <volition/OfferODBM.h>
#include <volition/StampODBM.h>
#include <volition/TransactionMaker.h>

namespace Volition {

//================================================================//
// Ledger_Inventory
//================================================================//

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::awardAssets ( AccountODBM& accountODBM, u64 inventoryNonce, const list < AssetBase >& assets, InventoryLogEntry& logEntry ) {

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    size_t quantity = assets.size ();
    if ( quantity == 0 ) return true;

    AbstractLedger& ledger = this->getLedger ();
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
        
        assetODBM.mOwner.set ( accountODBM.mAccountID );
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

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    if ( quantity == 0 ) return true;

    AbstractLedger& ledger = this->getLedger ();
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

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    AbstractLedger& ledger = this->getLedger ();
    
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

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    AbstractLedger& ledger = this->getLedger ();
    
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

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    AbstractLedger& ledger = this->getLedger ();
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

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    AbstractLedger& ledger = this->getLedger ();
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

    Psuedorandom prng ( digestEngine.digest ());
    
    map < string, size_t > awards;
    for ( size_t i = 0; i < quantity; ++i ) {
        u32 index = prng.randomInt32 ();
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

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    AbstractLedger& ledger = this->getLedger ();
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
LedgerResult Ledger_Inventory::cancelOffer ( AccountID accountID, string assetIdentifier, time_t time ) {

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    AbstractLedger& ledger = this->getLedger ();

    AssetID assetID ( assetIdentifier );
    if ( assetID.mIndex == AssetID::NULL_INDEX ) return "Invalid asset identifier.";
    
    AssetODBM assetODBM ( ledger, assetID );
    if ( !assetODBM ) return "Asset not found.";
    
    OfferID offerID = assetODBM.mOffer.get ();
    if ( offerID.mIndex == AssetID::NULL_INDEX ) return "Asset not offered for sale.";

    OfferODBM offerODBM ( ledger, assetODBM.mOffer.get ());
    if ( !offerODBM ) return "Asset marked for sale, but no offer found.";
    
    AccountODBM sellerODBM ( ledger, offerODBM.mSeller.get ());
    if ( !sellerODBM ) return "Seller not found.";

    if ( accountID != sellerODBM.mAccountID ) return "Cannot cancel another account's offer.";

    SerializableVector < AssetID::Index > assetIDs;
    offerODBM.mAssetIdentifiers.get ( assetIDs );
    this->clearOffers ( offerODBM.mSeller.get (), AssetListAdapter ( assetIDs.data (), assetIDs.size ()), time );

    offerODBM.mSeller.set ( OfferID::NULL_INDEX );

    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::clearOffers ( AccountID accountID, AssetListAdapter assetList, time_t time ) {

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    AbstractLedger& ledger = this->getLedger ();

    AccountODBM sellerODBM ( ledger, accountID );
    if ( sellerODBM.mAccountID == AccountID::NULL_INDEX ) return false;

    u64 inventoryNonce = sellerODBM.mInventoryNonce.get ( 0 );
    InventoryLogEntry logEntry ( time );
    
    // tag the assets and build the vector
    for ( size_t i = 0; i < assetList.size (); ++i ) {
        
        AssetODBM assetODBM ( ledger, assetList.getAssetIndex ( i ));
        
        assetODBM.mInventoryNonce.set ( inventoryNonce );
        assetODBM.mOffer.set ( OfferID::NULL_INDEX );
        
        logEntry.insertDeletion ( assetODBM.mAssetID );
        logEntry.insertAddition ( assetODBM.mAssetID );
    }
    
    ledger.updateInventory ( accountID, logEntry );
    
    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::clearInventory ( AccountID accountID, time_t time ) {

    AbstractLedger& ledger = this->getLedger ();

    if ( accountID == AccountID::NULL_INDEX ) return true;
    AccountODBM accountODBM ( ledger, accountID );

    InventoryLogEntry logEntry ( time );

    size_t totalAssets = accountODBM.mAssetCount.get ( 0 );    
    for ( size_t i = 0; i < totalAssets; ++i ) {
    
        AssetID::Index assetIndex = accountODBM.getInventoryField ( i ).get ();
        AssetODBM assetODBM ( ledger, assetIndex );

        // asset has no owner or position
        assetODBM.mOwner.set ( AssetID::NULL_INDEX );
        assetODBM.mInventoryNonce.set (( u64 )-1 );
        assetODBM.mPosition.set ( Asset::NULL_POSITION );
        
        logEntry.insertDeletion ( assetODBM.mAssetID );
    }

    accountODBM.mAssetCount.set ( 0 );
    ledger.updateInventory ( accountODBM.mAccountID, logEntry );

    return true;
}

//----------------------------------------------------------------//
void Ledger_Inventory::expireOffers ( time_t time ) {

    AbstractLedger& ledger = this->getLedger ();
    
    LedgerFieldODBM < u64 > globalOpenOfferCountField ( ledger, OfferODBM::keyFor_globalOpenOfferCount ());
    u64 count = globalOpenOfferCountField.get ( 0 );
    
    u64 keepCount = 0;
    for ( u64 i = 0; i < count; ++i ) {
        
        LedgerFieldODBM < u64 > globalOpenOfferListElementField ( ledger, OfferODBM::keyFor_globalOpenOfferListElement ( i ));
        
        OfferID offerID = globalOpenOfferListElementField.get ();
        OfferODBM offerODBM ( ledger, offerID );
        
        time_t expiration = Format::fromISO8601 ( offerODBM.mExpiration.get ());
        
        if ( expiration <= time ) {
        
            SerializableVector < AssetID::Index > assetIDs;
            offerODBM.mAssetIdentifiers.get ( assetIDs );
            this->clearOffers ( offerODBM.mSeller.get (), AssetListAdapter ( assetIDs.data (), assetIDs.size ()), time );
            
            offerODBM.mSeller.set ( OfferID::NULL_INDEX );
        }
        else {
            globalOpenOfferListElementField.set ( offerID );
            keepCount++;
        }
    }
    globalOpenOfferCountField.set ( keepCount );
}

//----------------------------------------------------------------//
void Ledger_Inventory::expireOffers2 ( time_t time ) {

    AbstractLedger& ledger = this->getLedger ();
    
    LedgerFieldODBM < u64 > globalOfferCountField ( ledger, OfferODBM::keyFor_globalOfferCount ());
    u64 count = globalOfferCountField.get ( 0 );
    
    u64 keepCount = 0;
    for ( u64 i = 0; i < count; ++i ) {
        
        OfferID offerID = i;
        OfferODBM offerODBM ( ledger, offerID );
        
        const AccountID buyerIndex = offerODBM.mBuyer.get ();
        if ( buyerIndex != AccountID::NULL_INDEX ) continue;
    
        const AccountID sellerIndex = offerODBM.mSeller.get ();
        if ( sellerIndex == AccountID::NULL_INDEX ) continue;
        
        time_t expiration = Format::fromISO8601 ( offerODBM.mExpiration.get ());
        
        if ( expiration <= time ) {
            SerializableVector < AssetID::Index > assetIDs;
            offerODBM.mAssetIdentifiers.get ( assetIDs );
            this->clearOffers ( offerODBM.mSeller.get (), AssetListAdapter ( assetIDs.data (), assetIDs.size ()), time );
            
            offerODBM.mSeller.set ( AccountID::NULL_INDEX );
        }
        else {
            LedgerFieldODBM < u64 > globalOpenOfferListElementField ( ledger, OfferODBM::keyFor_globalOpenOfferListElement ( keepCount ));
            globalOpenOfferListElementField.set ( offerID );
            keepCount++;
        }
    }
    LedgerFieldODBM < u64 > globalOpenOfferCountField ( ledger, OfferODBM::keyFor_globalOpenOfferCount ());
    globalOpenOfferCountField.set ( keepCount );
}

//----------------------------------------------------------------//
AssetID::Index Ledger_Inventory::getAssetID ( string assetID ) const {

    const AbstractLedger& ledger = this->getLedger ();

    AssetID::Index assetIndex = AssetID::decode ( assetID );
    
    AssetODBM assetODBM ( ledger, assetIndex );
    if ( assetODBM.mOwner.get () == AccountID::NULL_INDEX ) return AssetID::NULL_INDEX; // if an asset doesn't have an owner, it doesn't exist.
    
    return assetIndex;
}

//----------------------------------------------------------------//
void Ledger_Inventory::getInventory ( AccountID accountID, SerializableList < SerializableSharedConstPtr < Asset >>& assetList, size_t base, size_t count, bool sparse ) {

    AbstractLedger& ledger = this->getLedger ();

    SerializableList < Asset > assets;

    AccountODBM accountODBM ( ledger, accountID );
    if ( accountODBM.mAccountID == AccountID::NULL_INDEX ) return;

    size_t top = accountODBM.mAssetCount.get ( 0 );
    
    if ( count ) {
        size_t max = base + count;
        if ( max < top ) {
            top = max;
        }
    }
    
    for ( size_t i = base; i < top; ++i ) {
    
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
    this->getInventory ( accountID, inventory, 0, true );

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
LedgerResult Ledger_Inventory::offerAssets ( AccountID accountID, u64 minimumPrice, time_t expiration, AssetListAdapter assetList, time_t time ) {

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    if ( expiration <= time ) return "Offer already expired by record time.";

    AbstractLedger& ledger = this->getLedger ();

    AccountODBM sellerODBM ( ledger, accountID );
    if ( sellerODBM.mAccountID == AccountID::NULL_INDEX ) return "Count not find seller account.";

    LedgerFieldODBM < u64 > globalOfferCountField ( ledger, OfferODBM::keyFor_globalOfferCount ());
    u64 offerID = globalOfferCountField.get ( 0 );

    // check the assets
    for ( size_t i = 0; i < assetList.size (); ++i ) {
    
        string assetIdentifier = assetList.getAssetIdentifier ( i );
        AssetODBM assetODBM ( ledger, AssetID ( assetIdentifier ));
        
        if ( assetODBM.mAssetID == AssetID::NULL_INDEX )            return Format::write ( "Count not find asset %s.", assetIdentifier.c_str ());
        if ( assetODBM.mOwner.get () != sellerODBM.mAccountID )     return Format::write ( "Asset %s is not owned by %s.", assetIdentifier.c_str (), sellerODBM.mName.get ().c_str ());
        if ( assetODBM.mOffer.get () != OfferID::NULL_INDEX )       return Format::write ( "Asset %s already in an open offer.", assetIdentifier.c_str ());
    }

    u64 inventoryNonce = sellerODBM.mInventoryNonce.get ( 0 );
    InventoryLogEntry logEntry ( time );
    
    // tag the assets and build the vector
    SerializableVector < AssetID::Index > assetIDVector;
    for ( size_t i = 0; i < assetList.size (); ++i ) {
        
        AssetODBM assetODBM ( ledger, assetList.getAssetIndex ( i ));
        
        assetODBM.mInventoryNonce.set ( inventoryNonce );
        assetODBM.mOffer.set ( offerID ); // associate the asset with the offer
        
        logEntry.insertDeletion ( assetODBM.mAssetID );
        logEntry.insertAddition ( assetODBM.mAssetID );
        
        assetIDVector.push_back ( assetODBM.mAssetID );
    }
    
    ledger.updateInventory ( sellerODBM.mAccountID, logEntry );
    
    LedgerFieldODBM < u64 > globalOpenOfferCountField ( ledger, OfferODBM::keyFor_globalOpenOfferCount ());
    u64 offerPosition = globalOpenOfferCountField.get ( 0 );
    
    OfferODBM offerODBM ( ledger, offerID );

    offerODBM.mSeller.set ( accountID );
    offerODBM.mMinimumPrice.set ( minimumPrice );
    offerODBM.mExpiration.set ( Format::toISO8601 ( expiration ));
    offerODBM.mAssetIdentifiers.set ( assetIDVector );
    
    LedgerFieldODBM < u64 >( ledger, OfferODBM::keyFor_globalOpenOfferListElement ( offerPosition )).set ( offerID );
    
    globalOfferCountField.set ( offerID + 1 );
    globalOpenOfferCountField.set ( offerPosition + 1 );
    
    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::resetAssetFields ( AssetID::Index index, time_t time ) {

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    AbstractLedger& ledger = this->getLedger ();

    AssetODBM assetODBM ( ledger, index );
    if ( assetODBM.mAssetID == AssetID::NULL_INDEX ) return false;
    
    LedgerResult result = assetODBM.resetFields ();
    if ( !result ) return result;
    
    this->updateInventory ( assetODBM, time, InventoryLogEntry::EntryOp::UPDATE_ASSET );
    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::resetAssetFieldValue ( AssetID::Index index, string fieldName, time_t time ) {

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    AbstractLedger& ledger = this->getLedger ();
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
LedgerResult Ledger_Inventory::revokeAsset ( AssetID::Index index, time_t time ) {
    
    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    AbstractLedger& ledger = this->getLedger ();

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

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    AssetODBM assetODBM ( this->getLedger (), index );
    if ( assetODBM.mAssetID == AssetID::NULL_INDEX ) return "No such account.";
    
    LedgerResult result = assetODBM.setFieldValue ( fieldName, field );
    if ( !result ) return result;

    this->updateInventory ( assetODBM, time, InventoryLogEntry::EntryOp::UPDATE_ASSET );
    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::stampAssets ( AccountID accountID, AssetID stampID, u64 price, u64 version, AssetListAdapter assetList, time_t time ) {

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    if ( !assetList.size ()) return true;

    AbstractLedger& ledger = this->getLedger ();

    AccountODBM accountODBM ( ledger, accountID );
    if ( !accountODBM ) return "Could not find account.";
    
    StampODBM stampODBM ( ledger, stampID );
    if ( !stampODBM ) return "Could not find stamp.";
    
    if ( stampODBM.mVersion.get () != version ) return "Incorrect version specified.";
    
    AssetODBM stampAssetODBM ( ledger, stampID );
    assert ( stampAssetODBM ); // *must* exist. if not, how stamp?
    
    AccountID stampOwmerAccountID = stampAssetODBM.mOwner.get ();
    if ( stampOwmerAccountID == AccountID::NULL_INDEX ) return "This stamp is no longer available.";
    
    bool ownStamp = ( stampOwmerAccountID == accountID );
    
    u64 expectedPrice = ownStamp ? 0 : stampODBM.mPrice.get ();
    if ( expectedPrice != price ) return "Incorrect price specified.";
    
    u64 totalPrice = expectedPrice * assetList.size ();
    if ( totalPrice && ( accountODBM.mBalance.get () < totalPrice )) return "Insufficient funds.";
    
    shared_ptr < const Stamp > stamp = stampODBM.mBody.get ();
    assert ( stamp );
    
    u64 inventoryNonce = accountODBM.mInventoryNonce.get ( 0 );
    InventoryLogEntry logEntry ( time );
    
    // check and stamp all the assets
    for ( size_t i = 0; i < assetList.size (); ++i ) {
        
        string assetIdentifier = assetList.getAssetIdentifier ( i );
        AssetODBM assetODBM ( ledger, AssetID::decode ( assetIdentifier ));
        if ( assetODBM.mAssetID == AssetID::NULL_INDEX ) return Format::write ( "Count not find asset %s.", assetIdentifier.c_str ());
        if ( assetODBM.mOwner.get () != accountODBM.mAccountID ) return Format::write ( "Asset %s is not owned by %s.", assetIdentifier.c_str (), accountODBM.mName.get ().c_str ());
    
        shared_ptr < const Asset > asset = assetODBM.getAsset ();
        if ( !stamp->checkAsset ( asset )) return Format::write ( "Stamp cannot be applied to asset %s (asset failed to qualify).", assetIdentifier.c_str ());
        
        Stamp::Fields::const_iterator stampFieldIt = stamp->mFields.cbegin ();
        for ( ; stampFieldIt != stamp->mFields.cend (); ++stampFieldIt ) {
            LedgerResult result = assetODBM.setFieldValue ( stampFieldIt->first, stampFieldIt->second );
            if ( !result ) return result;
        }
        
        assetODBM.mInventoryNonce.set ( inventoryNonce );
        logEntry.insertUpdate ( assetODBM.mAssetID );
    }
    
    ledger.updateInventory ( accountODBM.mAccountID, logEntry );
    
    if ( totalPrice ) {
        AccountODBM stampOwnerAccountODBM ( ledger, stampOwmerAccountID );
        stampOwnerAccountODBM.addFunds ( totalPrice );
        accountODBM.subFunds ( totalPrice );
    }
    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::transferAssets ( AccountODBM& senderODBM, AccountODBM& receiverODBM, AssetListAdapter assetList, time_t time ) {
    
    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );
    
    AbstractLedger& ledger = this->getLedger ();

    if ( senderODBM.mAccountID == AccountID::NULL_INDEX )           return "Count not find sender account.";
    if ( receiverODBM.mAccountID == AccountID::NULL_INDEX )         return "Could not find recipient account.";
    if ( senderODBM.mAccountID == receiverODBM.mAccountID )         return "Cannot send assets to self.";

    size_t senderAssetCount = senderODBM.mAssetCount.get ( 0 );
    size_t receiverAssetCount = receiverODBM.mAssetCount.get ( 0 );

    shared_ptr < const Account > receiverAccount = receiverODBM.mBody.get ();
    Entitlements receiverEntitlements = ledger.getEntitlements < AccountEntitlements >( *receiverAccount );
    if ( !receiverEntitlements.check ( AccountEntitlements::MAX_ASSETS, receiverAssetCount + assetList.size ())) {
        double max = receiverEntitlements.resolvePathAs < NumericEntitlement >( AccountEntitlements::MAX_ASSETS )->getUpperLimit ().mLimit;
        return Format::write ( "Transaction would overflow receiving account's inventory limit of %d assets.", ( int )max );
    }
    
    InventoryLogEntry senderLogEntry ( time );
    InventoryLogEntry receiverLogEntry ( time );

    for ( size_t i = 0; i < assetList.size (); ++i, --senderAssetCount, ++receiverAssetCount ) {
        
        AssetODBM assetODBM ( ledger, assetList.getAssetIndex ( i ));
        
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
        assetODBM.mOffer.set ( OfferID::NULL_INDEX );
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
LedgerResult Ledger_Inventory::transferAssets ( AccountID senderAccountIndex, AccountID receiverAccountIndex, AssetListAdapter assetList, time_t time ) {
    
    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );
    
    AbstractLedger& ledger = this->getLedger ();
    
    AccountODBM senderODBM ( ledger, senderAccountIndex );
    AccountODBM receiverODBM ( ledger, receiverAccountIndex );
    
    // check all the assets
    for ( size_t i = 0; i < assetList.size (); ++i ) {
        string assetIdentifier = assetList.getAssetIdentifier ( i );
        AssetODBM assetODBM ( ledger, AssetID::decode ( assetIdentifier ));
        if ( assetODBM.mAssetID == AssetID::NULL_INDEX )            return Format::write ( "Count not find asset %s.", assetIdentifier.c_str ());
        if ( assetODBM.mOffer.get () != OfferID::NULL_INDEX )       return Format::write ( "Asset %s in an open offer.", assetIdentifier.c_str ());
        if ( assetODBM.mOwner.get () != senderODBM.mAccountID )     return Format::write ( "Asset %s is not owned by %s.", assetIdentifier.c_str (), senderODBM.mName.get ().c_str ());
    }
    
    this->transferAssets ( senderODBM, receiverODBM, assetList, time );
    
    return true;
}

//----------------------------------------------------------------//
void Ledger_Inventory::updateInventory ( AccountID accountID, const InventoryLogEntry& entry ) {

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    AbstractLedger& ledger = this->getLedger ();
    AccountODBM accountODBM ( ledger, accountID );

    u64 inventoryNonce = accountODBM.mInventoryNonce.get ( 0 );
    accountODBM.getInventoryLogEntryField ( inventoryNonce ).set ( entry );
    accountODBM.mInventoryNonce.set ( inventoryNonce + 1 );
}

//----------------------------------------------------------------//
void Ledger_Inventory::updateInventory ( AssetODBM& assetODBM, time_t time, InventoryLogEntry::EntryOp op ) {

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    AbstractLedger& ledger = this->getLedger ();

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

    LGN_LOG_SCOPE ( VOL_FILTER_LEDGER, INFO, __PRETTY_FUNCTION__ );

    AbstractLedger& ledger = this->getLedger ();
    const Schema& schema = ledger.getSchema ();
    
    AccountODBM accountODBM ( ledger, accountID );
    if ( accountODBM.mAccountID == AccountID::NULL_INDEX ) return "No such account.";

    // check the upgrades
    SerializableMap < string, string >::const_iterator upgradeIt = upgrades.cbegin ();
    for ( ; upgradeIt != upgrades.end (); ++upgradeIt ) {
        
        string assetID = upgradeIt->first;
        string upgradeType = upgradeIt->second;
        
        AssetODBM assetODBM ( ledger, AssetID::decode ( assetID ));

        if ( !assetODBM.mOwner.exists ())                                   return Format::write ( "Asset %s does not exist.", assetID.c_str ());
        if ( assetODBM.mOffer.get () != OfferID::NULL_INDEX )               return Format::write ( "Asset %s in an open offer.", assetID.c_str ());
        if ( assetODBM.mOwner.get () != accountODBM.mAccountID )            return Format::write ( "Asset %s does not belong to account %s.", assetID.c_str (), accountODBM.mName.get ().c_str ());
        if ( !schema.canUpgrade ( assetODBM.mType.get (), upgradeType ))    return Format::write (  "Cannot upgrade asset %s to %s.",  assetID.c_str (),  upgradeType.c_str ());
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
