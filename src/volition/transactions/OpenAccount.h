// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_OPEN_POLICY_H
#define VOLITION_TRANSACTIONS_OPEN_POLICY_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/AbstractSingleSignerTransaction.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// OpenAccount
//================================================================//
class OpenAccount :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "OPEN_ACCOUNT" )
    TRANSACTION_WEIGHT ( 1 )

    string      mAccountName;   // user provided name of account
    CryptoKey   mKey;           // master key
    string      mKeyName;       // name of master key
    u64         mAmount;        // amount to fund

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
        
        assert ( this->mKey );
        return ledger.openAccount ( this->mMakerSignature->getAccountName (), this->mAccountName, this->mAmount, this->mKeyName, this->mKey );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
