// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_AFFIRM_KEY_H
#define VOLITION_TRANSACTIONS_AFFIRM_KEY_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// AffirmKey
//================================================================//
class AffirmKey :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "AFFIRM_KEY" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string      mKeyName;
    CryptoKey   mKey;
    string      mPolicyName;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }

    //----------------------------------------------------------------//
    bool AbstractTransactionBody_apply ( Ledger& ledger ) const override {
    
        return ledger.affirmKey ( this->mMaker->getAccountName (), this->mKeyName, this->mKey, this->mPolicyName );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
