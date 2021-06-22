// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SENDVOL_H
#define VOLITION_TRANSACTIONS_SENDVOL_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// SendVOL
//================================================================//
class SendVOL :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "SEND_VOL" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string      mAccountName; // receiving account
    u64         mAmount;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        AbstractLedger& ledger = context.mLedger;
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::SEND_VOL )) return "Permission denied.";
        
        AccountODBM receiverODBM ( ledger, this->mAccountName );
        
        if ( !receiverODBM ) return "Could not find recipient account.";
        if ( context.mAccountID == receiverODBM.mAccountID ) return "Cannot send VOL to self.";
        
        receiverODBM.addFunds ( this->mAmount );
        context.mAccountODBM.subFunds ( this->mAmount );
        
        context.pushAccountLogEntry ( receiverODBM.mAccountID );
        
        return true;
    }
    
    //----------------------------------------------------------------//
    u64 AbstractTransactionBody_sendVOL () const override {
    
        return this->mAmount;
    }
    
    //----------------------------------------------------------------//
    SendVOL () :
        mAmount ( 0 ) {
    }
};

} // namespace Transactions
} // namespace Volition
#endif
