// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_UPGRADE_ASSETS_H
#define VOLITION_TRANSACTIONS_UPGRADE_ASSETS_H

#include <volition/common.h>
#include <volition/Format.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// UpgradeAssets
//================================================================//
class UpgradeAssets :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "UPGRADE_ASSETS" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    SerializableMap < string, string >      mUpgrades;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "upgrades",          this->mUpgrades );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "upgrades",          this->mUpgrades );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {

        if ( !context.mKeyEntitlements.check ( KeyEntitlements::UPGRADE_ASSETS )) return "Permission denied.";
        
        return context.mLedger.upgradeAssets (
            *context.mSchemaHandle,
            context.mAccount.mIndex,
            this->mUpgrades,
            context.mTime
        );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
