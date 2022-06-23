// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_RESTRICTKEY_H
#define VOLITION_TRANSACTIONS_RESTRICTKEY_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// RestrictKey
//================================================================//
class RestrictKey :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "RESTRICT_KEY" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    SerializableSharedPtr < Policy >    mPolicy;
    SerializableSharedPtr < Policy >    mBequest;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "bequest",       this->mBequest );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "bequest",       this->mBequest );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::RESTRICT_KEY )) return "Permission denied.";
        
        AbstractLedger& ledger = context.mLedger;
        KeyAndPolicy keyUpdated = context.mKeyAndPolicy;
        
        // restrict the policy
        if ( this->mPolicy ) {
        
            // new policy *must* be more restrictive than original
            if ( !ledger.isMoreRestrictivePolicy < KeyEntitlements >( *this->mPolicy, keyUpdated.mPolicy )) return false;
            keyUpdated.mPolicy = *this->mPolicy;
            
            // if there's a bequest, blow it away if it is *less* restrictive than the new policy
            if ( keyUpdated.mBequest && !ledger.isMoreRestrictivePolicy < KeyEntitlements >( *keyUpdated.mBequest, keyUpdated.mPolicy )) {
                keyUpdated.mBequest.reset ();
            }
        }
        
        if ( this->mBequest ) {
            if ( !ledger.isMoreRestrictivePolicy < KeyEntitlements >( *this->mBequest, keyUpdated.mPolicy )) return false;
            keyUpdated.mBequest = this->mBequest;
        }
        
        Account accountUpdated = context.mAccount;
        accountUpdated.mKeys [ this->mMaker.getKeyName ()] = keyUpdated;        
        context.mAccountODBM.mBody.set ( accountUpdated );
        
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
