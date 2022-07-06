// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_RENAMEACCOUNT_H
#define VOLITION_TRANSACTIONS_RENAMEACCOUNT_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/AccountODBM.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// RenameAccount
//================================================================//
class RenameAccount :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "RENAME_ACCOUNT" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string      mRevealedName;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "revealedName",  this->mRevealedName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "revealedName",  this->mRevealedName );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::RENAME_ACCOUNT )) return "Permission denied.";
        
        if ( this->mRevealedName.size () > 0 ) {
            return context.mLedger.renameAccount ( context.mAccountID, this->mRevealedName );
        }
        return "Missing parameters.";
    }
};

} // namespace Transactions
} // namespace Volition
#endif
