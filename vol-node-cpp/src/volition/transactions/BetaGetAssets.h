// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_BETA_GET_ASSETS_H
#define VOLITION_TRANSACTIONS_BETA_GET_ASSETS_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/AccountODBM.h>
#include <volition/InventoryLogEntry.h>
#include <volition/Schema.h>

namespace Volition {
namespace Transactions {

//================================================================//
// BetaGetAssets
//================================================================//
class BetaGetAssets :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "BETA_GET_ASSETS" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    u64         mNumAssets;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "numAssets",     this->mNumAssets );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "numAssets",     this->mNumAssets );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        Ledger& ledger = context.mLedger;
        const Account& account = context.mAccount;
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::BETA_GET_ASSETS )) return "Permission denied.";
        
        const Schema::Definitions& definitions = context.mSchemaHandle->getDefinitions ();
        size_t addedAssetCount = definitions.size () * ( size_t )this->mNumAssets;
        
        AccountODBM accountODBM ( ledger, account.mIndex );
        size_t assetCount = accountODBM.mAssetCount.get ( 0 );
        
        if ( !context.mAccountEntitlements.check ( AccountEntitlements::MAX_ASSETS, assetCount + addedAssetCount )) {
            double max = context.mAccountEntitlements.resolvePathAs < NumericEntitlement >( AccountEntitlements::MAX_ASSETS )->getUpperLimit ().mLimit;
            return Format::write ( "Transaction would overflow account inventory limit of %d assets.", ( int )max );
        }
        
        InventoryLogEntry logEntry;
        u64 inventoryNonce = accountODBM.mInventoryNonce.get ( 0 );
        
        Schema::Definitions::const_iterator definitionIt = definitions.cbegin ();
        for ( ; definitionIt != definitions.cend (); ++definitionIt ) {
            ledger.awardAssets ( *context.mSchemaHandle, accountODBM, inventoryNonce, definitionIt->first, ( size_t )this->mNumAssets, logEntry );
        }
        
        ledger.setInventoryLogEntry ( accountODBM.mIndex, inventoryNonce, logEntry );
        accountODBM.mInventoryNonce.set ( inventoryNonce + 1 );
        
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
