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

    string                              mKeyName;
    CryptoKey                           mKey;
    SerializableSharedPtr < Policy >    mPolicy;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
        serializer.serialize ( "policy",        this->mPolicy );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
        serializer.serialize ( "policy",        this->mPolicy );
    }

    //----------------------------------------------------------------//
    bool AbstractTransactionBody_apply ( TransactionContext& context ) const override {
    
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::AFFIRM_KEY )) return false;
    
        return context.mLedger.affirmKey (
            this->mMaker->getAccountName (),
            this->mMaker->getKeyName (),
            this->mKeyName,
            this->mKey,
            this->mPolicy.get ()
        );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
