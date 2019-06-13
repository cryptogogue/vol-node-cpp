// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_RUNSCRIPT_H
#define VOLITION_TRANSACTIONS_RUNSCRIPT_H

#include <volition/common.h>
#include <volition/AbstractSingleSignerTransaction.h>
#include <volition/AssetMethodInvocation.h>

namespace Volition {
namespace Transactions {

//================================================================//
// RunScript
//================================================================//
class RunScript :
    public AbstractSingleSignerTransaction {
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
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "weight",            this->mWeight );
        serializer.serialize ( "maturity",          this->mMaturity );
        serializer.serialize ( "invocations",       this->mInvocations );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "weight",            this->mWeight );
        serializer.serialize ( "maturity",          this->mMaturity );
        serializer.serialize ( "invocations",       this->mInvocations );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
        
        Invocations::const_iterator invocationIt = this->mInvocations.cbegin ();
        for ( ; invocationIt != this->mInvocations.cend (); ++invocationIt ) {
            if ( !ledger.invoke ( this->mMakerSignature->getAccountName (), *invocationIt )) return false;
        }
        return true;
    }
    
    //----------------------------------------------------------------//
    u64 AbstractTransaction_maturity () const override {
        
        return this->mWeight;
    }
    
    //----------------------------------------------------------------//
    u64 AbstractTransaction_weight () const override {
    
        return this->mMaturity;
    }
    
    //----------------------------------------------------------------//
    bool AbstractTransaction_verify ( const Ledger& ledger ) const override {
        
        if ( AbstractSingleSignerTransaction::AbstractTransaction_verify ( ledger )) {
        
            u64 totalWeight = MIN_WEIGHT;
            u64 maxMaturity = MIN_MATURITY;
        
            Invocations::const_iterator invocationIt = this->mInvocations.cbegin ();
            for ( ; invocationIt != this->mInvocations.cend (); ++invocationIt ) {
                if ( !ledger.verify ( *invocationIt )) return false;
                totalWeight += invocationIt->mWeight;
                
                if ( maxMaturity < invocationIt->mMaturity ) {
                    maxMaturity = invocationIt->mMaturity;
                }
            }
            
            if ( this->mWeight < totalWeight ) return false;
            if ( this->mMaturity < maxMaturity ) return false;
            
            return true;
        }
        return false;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
