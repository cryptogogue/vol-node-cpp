// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_RUNSCRIPT_H
#define VOLITION_TRANSACTIONS_RUNSCRIPT_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/AssetMethodInvocation.h>

namespace Volition {
namespace Transactions {

//================================================================//
// RunScript
//================================================================//
class RunScript :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "RUN_SCRIPT" )
    
    typedef SerializableVector < AssetMethodInvocation > Invocations;

    static const u64 MIN_WEIGHT     = 1;
    static const u64 MIN_MATURITY   = 0;

    u64             mWeight;
    u64             mMaturity;

    Invocations     mInvocations;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "weight",            this->mWeight );
        serializer.serialize ( "maturity",          this->mMaturity );
        serializer.serialize ( "invocations",       this->mInvocations );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "weight",            this->mWeight );
        serializer.serialize ( "maturity",          this->mMaturity );
        serializer.serialize ( "invocations",       this->mInvocations );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::RUN_SCRIPT )) return "Permission denied.";
        
        if ( !this->verifyMetrics ( context )) return "Transaction metrics are incorrect.";
        
        Invocations::const_iterator invocationIt = this->mInvocations.cbegin ();
        for ( ; invocationIt != this->mInvocations.cend (); ++invocationIt ) {
            TransactionResult result = context.mLedger.invoke ( this->mMaker.getAccountName (), *invocationIt, context.mTime );
            if ( !result ) return result;
        }
        return true;
    }
    
    //----------------------------------------------------------------//
    u64 AbstractTransactionBody_maturity () const override {
        
        return this->mMaturity;
    }
    
    //----------------------------------------------------------------//
    u64 AbstractTransactionBody_weight () const override {
    
        return this->mWeight;
    }
    
    //----------------------------------------------------------------//
    bool verifyMetrics ( TransactionContext& context ) const {
        
        u64 totalWeight = MIN_WEIGHT;
        u64 maxMaturity = MIN_MATURITY;
    
        Invocations::const_iterator invocationIt = this->mInvocations.cbegin ();
        for ( ; invocationIt != this->mInvocations.cend (); ++invocationIt ) {
            if ( !context.mLedger.verify ( *invocationIt )) return false;
            totalWeight += invocationIt->mWeight;
            
            if ( maxMaturity < invocationIt->mMaturity ) {
                maxMaturity = invocationIt->mMaturity;
            }
        }
        
        if ( this->mWeight != totalWeight ) return false;
        if ( this->mMaturity != maxMaturity ) return false;
        
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
