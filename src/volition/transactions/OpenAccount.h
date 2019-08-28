// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_OPEN_ACCOUNT_H
#define VOLITION_TRANSACTIONS_OPEN_ACCOUNT_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// OpenAccount
//================================================================//
class OpenAccount :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "OPEN_ACCOUNT" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string          mSuffix;        // child name formatted <hex3>.<hex3>.<hex3>
    CryptoKey       mKey;           // key
    u64             mGrant;         // amount to fund

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "suffix",        this->mSuffix );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "grant",         this->mGrant );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "suffix",        this->mSuffix );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "grant",         this->mGrant );
    }

    //----------------------------------------------------------------//
    bool AbstractTransactionBody_apply ( Ledger& ledger ) const override {
        
        assert ( this->mKey );
        return ledger.openAccount ( this->mMaker->getAccountName (), this->mSuffix, this->mGrant, this->mKey );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
