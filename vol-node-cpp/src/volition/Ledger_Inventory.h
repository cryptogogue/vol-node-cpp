// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGER_INVENTORY_H
#define VOLITION_LEDGER_INVENTORY_H

#include <volition/common.h>
#include <volition/AbstractLedgerComponent.h>
#include <volition/Asset.h>
#include <volition/LedgerResult.h>

namespace Volition {

class AccountODBM;
class InventoryLogEntry;
class Schema;

//================================================================//
// Ledger_Inventory
//================================================================//
class Ledger_Inventory :
    virtual public AbstractLedgerComponent {
public:

    //----------------------------------------------------------------//
    bool                                awardAssets                 ( const Schema& schema, Account::Index accountIndex, string assetType, size_t quantity );
    bool                                awardAssets                 ( const Schema& schema, AccountODBM& accountODBM, u64 inventoryNonce, string assetType, size_t quantity, InventoryLogEntry& logEntry );
    bool                                awardAssetsRandom           ( const Schema& schema, Account::Index accountIndex, string setOrDeckName, string seed, size_t quantity );
    shared_ptr < Asset >                getAsset                    ( const Schema& schema, AssetID::Index index, bool sparse = false ) const;
    void                                getInventory                ( const Schema& schema, Account::Index accountIndex, SerializableList < SerializableSharedPtr < Asset >>& assetList, size_t max = 0, bool sparse = false ) const;
    shared_ptr < InventoryLogEntry >    getInventoryLogEntry        ( Account::Index accountIndex, u64 inventoryNonce ) const;
    bool                                resetAssetFieldValue        ( const Schema& schema, AssetID::Index index, string fieldName );
    bool                                revokeAsset                 ( AssetID::Index index );
    bool                                setAssetFieldValue          ( const Schema& schema, AssetID::Index index, string fieldName, const AssetFieldValue& field );
    void                                setInventoryLogEntry        ( Account::Index accountIndex, u64 inventoryNonce, const InventoryLogEntry& entry );
    LedgerResult                        transferAssets              ( Account::Index senderAccountIndex, Account::Index receiverAccountIndex, const string* assetIdentifiers, size_t totalAssets );
    LedgerResult                        upgradeAssets               ( const Schema& schema, Account::Index accountIndex, const map < string, string >& upgrades );
};

} // namespace Volition
#endif
