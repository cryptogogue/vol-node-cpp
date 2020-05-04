// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGER_INVENTORY_H
#define VOLITION_LEDGER_INVENTORY_H

#include <volition/common.h>
#include <volition/AbstractLedgerComponent.h>
#include <volition/Asset.h>
#include <volition/LedgerResult.h>

namespace Volition {

class Schema;

//================================================================//
// Ledger_Inventory
//================================================================//
class Ledger_Inventory :
    virtual public AbstractLedgerComponent {
public:

    //----------------------------------------------------------------//
    bool                            awardAsset                  ( const Schema& schema, string accountName, string assetType, size_t quantity );
    bool                            awardAssetRandom            ( const Schema& schema, string accountName, string setOrDeckName, string seed, size_t quantity );
    shared_ptr < Asset >            getAsset                    ( const Schema& schema, AssetID::Index index ) const;
    SerializableList < Asset >      getInventory                ( const Schema& schema, string accountName, size_t max = 0 ) const;
    bool                            revokeAsset                 ( string accountName, AssetID::Index index );
    bool                            setAssetFieldValue          ( const Schema& schema, AssetID::Index index, string fieldName, const AssetFieldValue& field );
    LedgerResult                    transferAssets              ( string senderAccountName, string receiverAccountName, const string* assetIdentifiers, size_t totalAssets );
};

} // namespace Volition
#endif
