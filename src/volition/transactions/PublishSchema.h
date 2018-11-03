// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_PUBLISH_SCHEMA_H
#define VOLITION_TRANSACTIONS_PUBLISH_SCHEMA_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/AbstractSingleSignerTransaction.h>
#include <volition/Schema.h>

namespace Volition {
namespace Transactions {

//================================================================//
// PublishSchema
//================================================================//
class PublishSchema :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "PUBLISH_SCHEMA" )
    TRANSACTION_WEIGHT ( 1 )

    string                                  mSchemaName;
    string                                  mJSON;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "name",          this->mSchemaName );
        serializer.serialize ( "json",          this->mJSON );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "name",          this->mSchemaName );
        serializer.serialize ( "json",          this->mJSON );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
    
        return ledger.publishSchema ( this->mSchemaName, this->mJSON );
    }
    
    //----------------------------------------------------------------//
    bool AbstractTransaction_checkSignature  ( Ledger& ledger ) const override {

        return true;
    }
    
    //----------------------------------------------------------------//
    void AbstractTransaction_incrementNonce ( Ledger& ledger ) const override {
    }
};

} // namespace Transactions
} // namespace Volition
#endif
