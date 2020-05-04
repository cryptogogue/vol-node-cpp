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
#include <volition/Ledger_Inventory.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/LuaContext.h>
#include <volition/TransactionMaker.h>

namespace Volition {

//================================================================//
// Ledger_Inventory
//================================================================//

//----------------------------------------------------------------//
bool Ledger_Inventory::awardAsset ( const Schema& schema, AccountODBM& accountODBM, u64 inventoryNonce, string assetType, size_t quantity ) {

    Ledger& ledger = this->getLedger ();

    if ( quantity == 0 ) return true;
    if ( !schema.getDefinitionOrNull ( assetType )) return false;

    LedgerKey KEY_FOR_GLOBAL_ASSET_COUNT = Ledger::keyFor_globalAssetCount ();
    size_t globalAssetCount = ledger.getValueOrFallback < size_t >( KEY_FOR_GLOBAL_ASSET_COUNT, 0 );
    size_t accountAssetCount = accountODBM.mAssetCount.get ( 0 );
    
    for ( size_t i = 0; i < quantity; ++i ) {
        
        AssetODBM assetODBM ( ledger, globalAssetCount + i );
                
        assetODBM.mOwner.set ( accountODBM.mIndex );
        assetODBM.mInventoryNonce.set ( inventoryNonce );
        assetODBM.mPosition.set ( accountAssetCount + i );
        assetODBM.mType.set ( assetType );
        
        accountODBM.getInventoryField ( assetODBM.mPosition.get ()).set ( assetODBM.mIndex );
    }
    
    ledger.setValue < size_t >( KEY_FOR_GLOBAL_ASSET_COUNT, globalAssetCount + quantity );
    accountODBM.mAssetCount.set ( accountAssetCount + quantity );

    return true;
}

//----------------------------------------------------------------//
bool Ledger_Inventory::awardAsset ( const Schema& schema, string accountName, string assetType, size_t quantity ) {

    Ledger& ledger = this->getLedger ();
    
    AccountODBM accountODBM ( ledger, ledger.getAccountIndex ( accountName ));
    if ( accountODBM.mIndex == Account::NULL_INDEX ) return false;
    u64 inventoryNonce = accountODBM.mInventoryNonce.get ( 0 );

    this->awardAsset ( schema, accountODBM, inventoryNonce, assetType, quantity );
   
    accountODBM.mInventoryNonce.set ( inventoryNonce + 1 );

    return true;
}

//----------------------------------------------------------------//
bool Ledger_Inventory::awardAssetRandom ( const Schema& schema, string accountName, string setOrDeckName, string seed, size_t quantity ) {

    Ledger& ledger = this->getLedger ();

    if ( quantity == 0 ) return 0;

    const Schema::Deck* setOrDeck = schema.getSetOrDeck ( setOrDeckName );
    if ( !setOrDeck ) return 0;

    AccountODBM accountODBM ( ledger, ledger.getAccountIndex ( accountName ));
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
    
    u64 inventoryNonce = accountODBM.mInventoryNonce.get ( 0 );
    
    map < string, size_t >::const_iterator awardIt = awards.cbegin ();
    for ( ; awardIt != awards.cend (); ++awardIt ) {
        ledger.awardAsset ( schema, accountODBM, inventoryNonce, awardIt->first, awardIt->second );
    }
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
        
//        if ( field.mMutable ) {
//
//            string keyforAssetModifiedField = ledger.formatKeyforAssetModifiedField ( identifier, fieldName );
//
//            switch ( field.mType ) {
//                case AssetTemplateField::Type::NUMERIC:
//                    value = ledger.getValueOrFallback < double >( keyforAssetModifiedField, value.mNumeric );
//                    break;
//                case AssetTemplateField::Type::STRING:
//                    value = ledger.getValueOrFallback < string >( keyforAssetModifiedField, value.mString );
//                    break;
//            }
//            asset->mFields [ fieldName ] = value;
//        }
        asset->mFields [ fieldName ] = value;
    }
    return asset;
}

//----------------------------------------------------------------//
SerializableList < Asset > Ledger_Inventory::getInventory ( const Schema& schema, string accountName, size_t max ) const {

    const Ledger& ledger = this->getLedger ();

    SerializableList < Asset > assets;

    AccountODBM accountODBM ( ledger, ledger.getAccountIndex ( accountName ));
    if ( accountODBM.mIndex == Account::NULL_INDEX ) return assets;

    size_t assetCount = accountODBM.mAssetCount.get ();
    
    if (( max > 0 ) && ( max < assetCount )) {
        assetCount = max;
    }
    
    for ( size_t i = 0; i < assetCount; ++i ) {
    
        AssetID::Index assetIndex = accountODBM.getInventoryField ( i ).get ();
        shared_ptr < Asset > asset = ledger.getAsset ( schema, assetIndex );
        assert ( asset );
        assets.push_back ( *asset );
    }
    return assets;
}

//----------------------------------------------------------------//
bool Ledger_Inventory::revokeAsset ( string accountName, AssetID::Index index ) {

    Ledger& ledger = this->getLedger ();

    // make sure the account exists
    AccountODBM accountODBM ( ledger, ledger.getAccountIndex ( accountName ));
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
    return true;
}

//----------------------------------------------------------------//
bool Ledger_Inventory::setAssetFieldValue ( const Schema& schema, AssetID::Index index, string fieldName, const AssetFieldValue& field ) {

    Ledger& ledger = this->getLedger ();

    // make sure the asset exists
    LedgerKey KEY_FOR_ASSET_TYPE = AssetODBM::keyFor_type ( index );
    if ( !ledger.hasValue ( KEY_FOR_ASSET_TYPE )) return false;
    string assetType = ledger.getValue < string >( KEY_FOR_ASSET_TYPE );

    // make sure the field exists
    const AssetDefinition* assetDefinition = schema.getDefinitionOrNull ( assetType );
    if ( !assetDefinition ) return false;
    if ( !assetDefinition->hasMutableField ( fieldName, field.getType ())) return false;

//    // set the field
//    string keyforAssetModifiedField = Ledger::formatKeyforAssetModifiedField ( identifier, fieldName );
//
//    switch ( field.mType ) {
//        case AssetTemplateField::Type::NUMERIC:
//            ledger.setValue < double >( keyforAssetModifiedField, field.mValue.mNumeric );
//            break;
//        case AssetTemplateField::Type::STRING:
//            ledger.setValue < string >( keyforAssetModifiedField, field.mValue.mString );
//            break;
//    }

    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Inventory::transferAssets ( string senderAccountName, string receiverAccountName, const string* assetIdentifiers, size_t totalAssets ) {
    
    Ledger& ledger = this->getLedger ();
    
    AccountODBM senderODBM ( ledger, ledger.getAccountIndex ( senderAccountName ));
    AccountODBM receiverODBM ( ledger, ledger.getAccountIndex ( receiverAccountName ));

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
        if ( assetODBM.mOwner.get () != senderODBM.mIndex ) return Format::write ( "Asset %s is not owned by %s.", assetIdentifier.c_str (), senderAccountName.c_str ());
    }
    
    u64 inventoryNonce = receiverODBM.mInventoryNonce.get ( 0 );

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
        assetODBM.mInventoryNonce.set ( inventoryNonce );
        assetODBM.mPosition.set ( receiverAssetCount );
        receiverODBM.getInventoryField ( assetODBM.mPosition.get ()).set ( assetODBM.mIndex );
    }
    
    senderODBM.mAssetCount.set ( senderAssetCount );
    receiverODBM.mAssetCount.set ( receiverAssetCount );
    receiverODBM.mInventoryNonce.set ( inventoryNonce + 1 );
    
    return true;
}

} // namespace Volition
