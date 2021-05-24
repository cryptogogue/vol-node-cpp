// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGER_INVENTORY_H
#define VOLITION_LEDGER_INVENTORY_H

#include <volition/common.h>
#include <volition/AbstractLedgerComponent.h>
#include <volition/Asset.h>
#include <volition/InventoryLogEntry.h>
#include <volition/LedgerResult.h>

namespace Volition {

class AccountODBM;
class AssetODBM;
class Schema;

//================================================================//
// AssetListAdapter
//================================================================//
// Just a helper class to make writing functions that acceps lists of assets easier.
// Adapts lists/vectors of asset IDs of names.
class AssetListAdapter {
private:

    const string*           mAssetIDStrings;
    const AssetID::Index*   mAssetIDs;
    size_t                  mTotalAssets;

public:

    //----------------------------------------------------------------//
    AssetListAdapter ( const string* assetIDs, size_t totalAssets ) :
        mAssetIDStrings ( assetIDs ),
        mAssetIDs ( NULL ),
        mTotalAssets ( totalAssets ) {
    }
    
    //----------------------------------------------------------------//
    AssetListAdapter ( const AssetID::Index* assetIDs, size_t totalAssets ) :
        mAssetIDStrings ( NULL ),
        mAssetIDs ( assetIDs ),
        mTotalAssets ( totalAssets ) {
    }
    
    //----------------------------------------------------------------//
    AssetID::Index getAssetIndex ( size_t i ) {
        assert ( i < this->mTotalAssets );
        return this->mAssetIDs ? this->mAssetIDs [ i ] : AssetID::decode ( this->mAssetIDStrings [ i ]);
    }
    
    //----------------------------------------------------------------//
    string getAssetIdentifier ( size_t i ) {
        assert ( i < this->mTotalAssets );
        return this->mAssetIDs ? AssetID::encode ( this->mAssetIDs [ i ]) : this->mAssetIDStrings [ i ];
    }
    
    //----------------------------------------------------------------//
    size_t size () {
        return this->mTotalAssets;
    }
};

//================================================================//
// Ledger_Inventory
//================================================================//
class Ledger_Inventory :
    virtual public AbstractLedgerComponent {
private:

    //----------------------------------------------------------------//
    LedgerResult                        awardAssets                 ( AccountODBM& accountODBM, u64 inventoryNonce, const list < AssetBase >& assets, InventoryLogEntry& logEntry );
    LedgerResult                        awardAssets                 ( AccountODBM& accountODBM, u64 inventoryNonce, string assetType, size_t quantity, InventoryLogEntry& logEntry );
    LedgerResult                        clearOffers                 ( AccountID accountID, AssetListAdapter assetList, time_t time );
    LedgerResult                        transferAssets              ( AccountODBM& senderODBM, AccountODBM& receiverODBM, AssetListAdapter assetList, time_t time );
    void                                updateInventory             ( AccountID accountID, const InventoryLogEntry& entry );
    void                                updateInventory             ( AssetODBM& assetODBM, time_t time, InventoryLogEntry::EntryOp op );

public:

    //----------------------------------------------------------------//
    LedgerResult                        awardAssets                 ( AccountID accountID, string assetType, size_t quantity, time_t time );
    LedgerResult                        awardAssets                 ( AccountID accountID, const list < AssetBase >& assets, time_t time );
    LedgerResult                        awardAssetsAll              ( AccountID accountID, size_t quantity, time_t time );
    LedgerResult                        awardAssetsRandom           ( AccountID accountID, string deckName, string seed, size_t quantity, time_t time );
    LedgerResult                        awardDeck                   ( AccountID accountID, string deckName, time_t time );
    LedgerResult                        buyAssets                   ( AccountID accountID, string assetIdentifier, u64 price, time_t time );
    LedgerResult                        clearInventory              ( AccountID accountID, time_t time );
    void                                expireOffers                ( time_t time );
    AssetID::Index                      getAssetID                  ( string assetID ) const;
    void                                getInventory                ( AccountID accountID, SerializableList < SerializableSharedConstPtr < Asset >>& assetList, size_t base = 0, size_t count = 0, bool sparse = false );
    map < string, size_t >              getInventoryHistogram       ( AccountID accountID );
    LedgerResult                        offerAssets                 ( AccountID accountID, u64 minimumPrice, time_t expiration, AssetListAdapter assetList, time_t time );
    bool                                resetAssetFieldValue        ( AssetID::Index index, string fieldName, time_t time );
    bool                                revokeAsset                 ( AssetID::Index index, time_t time  );
    LedgerResult                        setAssetFieldValue          ( AssetID::Index index, string fieldName, const AssetFieldValue& field, time_t time );
    LedgerResult                        transferAssets              ( AccountID senderAccountIndex, AccountID receiverAccountIndex, AssetListAdapter assetList, time_t time );
    LedgerResult                        upgradeAssets               ( AccountID accountID, const map < string, string >& upgrades, time_t time );
};

} // namespace Volition
#endif
