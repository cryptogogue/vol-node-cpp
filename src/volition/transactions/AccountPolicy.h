// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_ACCOUNT_POLICY_H
#define VOLITION_TRANSACTIONS_ACCOUNT_POLICY_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// AccountPolicy
//================================================================//
class AccountPolicy :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "ACCOUNT_POLICY" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string                              mPolicyName;
    SerializableUniquePtr < Policy >    mPolicy;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }

    //----------------------------------------------------------------//
    bool AbstractTransactionBody_apply ( Ledger& ledger ) const override {
        UNUSED ( ledger );
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
