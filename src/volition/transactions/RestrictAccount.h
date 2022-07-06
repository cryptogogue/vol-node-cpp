// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_RESTRICTACCOUNT_H
#define VOLITION_TRANSACTIONS_RESTRICTACCOUNT_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// RestrictAccount
//================================================================//
class RestrictAccount :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "RESTRICT_ACCOUNT" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    SerializableSharedPtr < Policy >    mPolicy;
    SerializableSharedPtr < Policy >    mBequest;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "bequest",       this->mBequest );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "bequest",       this->mBequest );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::RESTRICT_ACCOUNT )) return "Permission denied.";
        
        AbstractLedger& ledger = context.mLedger;
        Account accountUpdated = context.mAccount;
        
        // restrict the policy
        if ( this->mPolicy ) {
        
            // new policy *must* be more restrictive than original
            if ( !ledger.isMoreRestrictivePolicy < AccountEntitlements >( *this->mPolicy, accountUpdated.mPolicy )) return false;
            accountUpdated.mPolicy = *this->mPolicy;
            
            // if there's a bequest, blow it away if it is *less* restrictive than the new policy
            if ( accountUpdated.mBequest && !ledger.isMoreRestrictivePolicy < AccountEntitlements >( *accountUpdated.mBequest, accountUpdated.mPolicy )) {
                accountUpdated.mBequest.reset ();
            }
        }
        
        if ( this->mBequest ) {
            if ( !ledger.isMoreRestrictivePolicy < AccountEntitlements >( *this->mBequest, accountUpdated.mPolicy )) return false;
            accountUpdated.mBequest = this->mBequest;
        }
        
        context.mAccountODBM.mBody.set ( accountUpdated );
        
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
