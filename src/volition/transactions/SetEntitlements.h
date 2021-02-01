// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SET_POLICY_H
#define VOLITION_TRANSACTIONS_SET_POLICY_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// SetEntitlements
//================================================================//
class SetEntitlements :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "SET_ENTITLEMENTS" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string          mName;
    Entitlements    mEntitlements;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "name",              this->mName );
        serializer.serialize ( "entitlements",      this->mEntitlements  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "name",              this->mName );
        serializer.serialize ( "entitlements",      this->mEntitlements  );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
                
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::SET_ENTITLEMENTS )) return "Permission denied.";
        context.mLedger.setEntitlements ( this->mName, this->mEntitlements );
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
