// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SEND_ASSETS_H
#define VOLITION_TRANSACTIONS_SEND_ASSETS_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/AssetODBM.h>
#include <volition/Format.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// SendAssets
//================================================================//
class SendAssets :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "SEND_ASSETS" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string                          mAccountName;
    SerializableVector < string >   mAssetIdentifiers;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "accountName",               this->mAccountName );
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "accountName",               this->mAccountName );
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        Ledger& ledger = context.mLedger;
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::SEND_ASSETS )) return "Permission denied.";
        
        string senderAccountName = context.mAccount.mName;
        AccountODBM senderODBM ( ledger, context.mAccount.mIndex );
        AccountODBM receiverODBM ( ledger, ledger.getAccountIndex ( this->mAccountName ));

        if ( senderODBM.mIndex == Account::NULL_INDEX ) return "Count not find sender account.";
        if ( receiverODBM.mIndex == Account::NULL_INDEX ) return "Could not find recipient account.";
        if ( senderODBM.mIndex == receiverODBM.mIndex ) return "Cannot send assets to self.";

        size_t senderAssetCount = senderODBM.mAssetCount.get ( 0 );
        size_t receiverAssetCount = receiverODBM.mAssetCount.get ( 0 );

        const string* assetIdentifiers = this->mAssetIdentifiers.data ();
        size_t totalAssets = this->mAssetIdentifiers.size ();

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
            assetODBM.mPosition.set ( receiverAssetCount );
            receiverODBM.getInventoryField ( assetODBM.mPosition.get ()).set ( assetODBM.mIndex );
        }
        
        senderODBM.mAssetCount.set ( senderAssetCount );
        receiverODBM.mAssetCount.set ( receiverAssetCount );
        
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
