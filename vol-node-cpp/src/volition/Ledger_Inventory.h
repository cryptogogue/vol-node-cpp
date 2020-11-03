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
// Ledger_Inventory
//================================================================//
class Ledger_Inventory :
    virtual public AbstractLedgerComponent {
private:

    //----------------------------------------------------------------//
    LedgerResult                        awardAssets                 ( AccountODBM& accountODBM, u64 inventoryNonce, const SerializableList < SerializableSharedConstPtr < Asset >>& assets, InventoryLogEntry& logEntry );
    LedgerResult                        awardAssets                 ( AccountODBM& accountODBM, u64 inventoryNonce, string assetType, size_t quantity, InventoryLogEntry& logEntry );
    void                                updateInventory             ( AccountID accountID, const InventoryLogEntry& entry );
    void                                updateInventory             ( AssetODBM& assetODBM, time_t time, InventoryLogEntry::EntryOp op );

public:

    //----------------------------------------------------------------//
    LedgerResult                        awardAssets                 ( AccountID accountID, string assetType, size_t quantity, time_t time );
    LedgerResult                        awardAssets                 ( AccountID accountID, const SerializableList < SerializableSharedConstPtr < Asset >>& assets, time_t time );
    LedgerResult                        awardAssetsAll              ( AccountID accountID, size_t quantity, time_t time );
    LedgerResult                        awardAssetsRandom           ( AccountID accountID, string deckName, string seed, size_t quantity, time_t time );
    LedgerResult                        awardDeck                   ( AccountID accountID, string deckName, time_t time );
    AssetID::Index                      getAssetID                  ( string assetID ) const;
    void                                getInventory                ( AccountID accountID, SerializableList < SerializableSharedConstPtr < Asset >>& assetList, size_t max = 0, bool sparse = false ) const;
    map < string, size_t >              getInventoryHistogram       ( AccountID accountID );
    bool                                resetAssetFieldValue        ( AssetID::Index index, string fieldName, time_t time );
    bool                                revokeAsset                 ( AssetID::Index index, time_t time  );
    LedgerResult                        setAssetFieldValue          ( AssetID::Index index, string fieldName, const AssetFieldValue& field, time_t time );
    LedgerResult                        transferAssets              ( AccountID senderAccountIndex, AccountID receiverAccountIndex, const string* assetIdentifiers, size_t totalAssets, time_t time );
    LedgerResult                        upgradeAssets               ( AccountID accountID, const map < string, string >& upgrades, time_t time );
};

} // namespace Volition
#endif
