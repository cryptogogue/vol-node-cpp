// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SETIDENTITYPROVIDER_H
#define VOLITION_TRANSACTIONS_SETIDENTITYPROVIDER_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/ContractWithDigest.h>
#include <volition/IdentityProvider.h>
#include <volition/Munge.h>
#include <volition/Policy.h>
#include <volition/Signature.h>

namespace Volition {
namespace Transactions {

//================================================================//
// SetIdentityProvider
//================================================================//
class SetIdentityProvider :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "SET_IDENTITY_PROVIDER" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )
    TRANSACTION_MIN_RELEASE ( 3 )

    string              mName;
    IdentityProvider    mIdentityProvider;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "name",                  this->mName );
        serializer.serialize ( "identityProvider",      this->mIdentityProvider );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "name",                  this->mName );
        serializer.serialize ( "identityProvider",      this->mIdentityProvider );
    }
    
    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::SET_IDENTITY_PROVIDER )) return "Permission denied.";

        if ( !this->mIdentityProvider.mKeyPolicy.isValid ( context.mLedger ))         return "Invalid key policy.";
        if ( !this->mIdentityProvider.mAccountPolicy.isValid ( context.mLedger ))     return "Invalid account policy.";

        context.mLedger.setIdentityProvider ( this->mName, this->mIdentityProvider );

        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
