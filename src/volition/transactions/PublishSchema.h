// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_PUBLISH_SCHEMA_H
#define VOLITION_TRANSACTIONS_PUBLISH_SCHEMA_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Schema.h>

namespace Volition {
namespace Transactions {

//================================================================//
// PublishSchema
//================================================================//
class PublishSchema :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "PUBLISH_SCHEMA" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string      mSchemaName;
    Schema      mSchema;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "name",          this->mSchemaName );
        serializer.serialize ( "schema",        this->mSchema );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "name",          this->mSchemaName );
        serializer.serialize ( "schema",        this->mSchema );
    }

    //----------------------------------------------------------------//
    bool AbstractTransactionBody_apply ( Ledger& ledger ) const override {
        
        return ledger.publishSchema ( this->mMaker->getAccountName (), this->mSchemaName, this->mSchema );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
