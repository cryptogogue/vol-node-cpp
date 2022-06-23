// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SENDASSETS_H
#define VOLITION_TRANSACTIONS_SENDASSETS_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/AssetTransferDetails.h>
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
                
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::SEND_ASSETS )) return "Permission denied.";
        
        AccountID receiverID = context.mLedger.getAccountID ( this->mAccountName );
        context.pushAccountLogEntry ( receiverID );
        
        return context.mLedger.transferAssets (
            context.mAccountID,
            receiverID,
            AssetListAdapter (
                this->mAssetIdentifiers.data (),
                this->mAssetIdentifiers.size ()
            ),
            context.mTime
        );
    }
    
    //----------------------------------------------------------------//
    TransactionDetailsPtr AbstractTransactionBody_getDetails ( const AbstractLedger& ledger ) const override {
        
        AccountODBM senderODBM ( ledger, this->getMakerAccountName ());
        if ( !senderODBM ) return NULL;
        
        AccountODBM receiverODBM ( ledger, this->mAccountName );
        if ( !receiverODBM ) return NULL;
        
        shared_ptr < AssetTransferDetails > details = make_shared < AssetTransferDetails >();
        
        AssetListAdapter assetList ( this->mAssetIdentifiers.data (), this->mAssetIdentifiers.size ());
        for ( size_t i = 0; i < assetList.size (); ++i ) {
        
            AssetODBM assetODBM ( ledger, assetList.getAssetIndex ( i ));
            shared_ptr < const Asset > asset = assetODBM.getAsset ();
            if ( !asset ) return NULL;
            details->mAssets.push_back ( asset );
        }
        
        details->mFrom      = senderODBM.mName.get ();
        details->mFromID    = senderODBM.mAccountID;
        
        details->mTo        = receiverODBM.mName.get ();
        details->mToID      = receiverODBM.mAccountID;
        
        return details;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
