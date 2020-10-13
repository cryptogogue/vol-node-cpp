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
    LedgerResult                        awardAssets                 ( const Schema& schema, AccountODBM& accountODBM, u64 inventoryNonce, string assetType, size_t quantity, InventoryLogEntry& logEntry );
    void                                updateInventory             ( Account::Index accountIndex, const InventoryLogEntry& entry );
    void                                updateInventory             ( AssetODBM& assetODBM, time_t time, InventoryLogEntry::EntryOp op );

public:

    //----------------------------------------------------------------//
    LedgerResult                        awardAssets                 ( const Schema& schema, Account::Index accountIndex, string assetType, size_t quantity, time_t time );
    LedgerResult                        awardAssetsAll              ( const Schema& schema, Account::Index accountIndex, size_t quantity, time_t time );
    LedgerResult                        awardAssetsRandom           ( const Schema& schema, Account::Index accountIndex, string deckName, string seed, size_t quantity, time_t time );
    LedgerResult                        awardDeck                   ( const Schema& schema, Account::Index accountIndex, string deckName, time_t time );
    AssetID::Index                      getAssetID                  ( string assetID ) const;
    void                                getInventory                ( const Schema& schema, Account::Index accountIndex, SerializableList < SerializableSharedConstPtr < Asset >>& assetList, size_t max = 0, bool sparse = false ) const;
    bool                                resetAssetFieldValue        ( const Schema& schema, AssetID::Index index, string fieldName, time_t time );
    bool                                revokeAsset                 ( AssetID::Index index, time_t time  );
    LedgerResult                        setAssetFieldValue          ( const Schema& schema, AssetID::Index index, string fieldName, const AssetFieldValue& field, time_t time );
    LedgerResult                        transferAssets              ( Account::Index senderAccountIndex, Account::Index receiverAccountIndex, const string* assetIdentifiers, size_t totalAssets, time_t time );
    LedgerResult                        upgradeAssets               ( const Schema& schema, Account::Index accountIndex, const map < string, string >& upgrades, time_t time );
};

} // namespace Volition
#endif
