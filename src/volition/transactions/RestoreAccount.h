// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_RESTORE_ACCOUNT_H
#define VOLITION_TRANSACTIONS_RESTORE_ACCOUNT_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// RestoreAccount
//================================================================//
class RestoreAccount :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "RESTORE_ACCOUNT" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string                              mName;
    u64                                 mBalance;
    SerializableSharedPtr < Policy >    mPolicy;
    SerializableSharedPtr < Policy >    mBequest;

    SerializableMap < string, KeyAndPolicy >                mKeys;
    SerializableList < SerializableSharedPtr < Asset >>     mInventory;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "name",          this->mName );
        serializer.serialize ( "balance",       this->mBalance );
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "bequest",       this->mBequest );
        serializer.serialize ( "keys",          this->mKeys );
        serializer.serialize ( "inventory",     this->mInventory );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "name",          this->mName );
        serializer.serialize ( "balance",       this->mBalance );
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "bequest",       this->mBequest );
        serializer.serialize ( "keys",          this->mKeys );
        serializer.serialize ( "inventory",     this->mInventory );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::RESTORE_ACCOUNT )) return "Permission denied.";
        
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
