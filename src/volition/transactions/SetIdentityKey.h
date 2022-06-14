// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SETIDENTITYKEY_H
#define VOLITION_TRANSACTIONS_SETIDENTITYKEY_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/ContractWithDigest.h>
#include <volition/Munge.h>
#include <volition/Policy.h>
#include <volition/Signature.h>

namespace Volition {
namespace Transactions {

//================================================================//
// SetIdentityKey
//================================================================//
class SetIdentityKey :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "SET_IDENTITY_KEY" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string          mKeyName;
    string          mEd25519PublicHex;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "keyName",           this->mKeyName );
        serializer.serialize ( "ed25519PublicHex",  this->mEd25519PublicHex );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "keyName",           this->mKeyName );
        serializer.serialize ( "ed25519PublicHex",  this->mEd25519PublicHex );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::SET_IDENTITY_KEY )) return "Permission denied.";

        context.mLedger.setIdentityKey ( this->mKeyName, this->mEd25519PublicHex );

        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
