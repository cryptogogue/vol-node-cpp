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

    string      mRevealedName;
    Digest      mNameHash;      // SHA256 ( "<new name>" )
    Digest      mNameSecret;    // SHA256 ( "<current name>:<new name>" )

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "revealedName",  this->mRevealedName );
        serializer.serialize ( "nameHash",      this->mNameHash );
        serializer.serialize ( "nameSecret",    this->mNameSecret );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "revealedName",  this->mRevealedName );
        serializer.serialize ( "nameHash",      this->mNameHash );
        serializer.serialize ( "nameSecret",    this->mNameSecret );
    }

    //----------------------------------------------------------------//
    bool AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::RENAME_ACCOUNT )) return false;
        
        return context.mLedger.renameAccount ( this->mMaker->getAccountName (), this->mRevealedName, this->mNameHash, this->mNameSecret );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
