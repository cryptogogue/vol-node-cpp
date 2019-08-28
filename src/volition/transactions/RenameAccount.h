// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_RENAME_ACCOUNT_H
#define VOLITION_TRANSACTIONS_RENAME_ACCOUNT_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// RenameAccount
//================================================================//
class RenameAccount :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "RENAME_ACCOUNT" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string      mNewName;       // new name for account
    string      mSecretName;    // secret name (or fallback)

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "newName",       this->mNewName );
        serializer.serialize ( "secretName",    this->mSecretName  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "newName",       this->mNewName );
        serializer.serialize ( "secretName",    this->mSecretName  );
    }

    //----------------------------------------------------------------//
    bool AbstractTransactionBody_apply ( Ledger& ledger ) const override {
        
        assert ( this->mKey );
        return ledger.renameAccount ( this->mMaker->getAccountName (), this->mNewName, this->mSecretName );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
