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

    Schema      mSchema;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "schema",        this->mSchema );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "schema",        this->mSchema );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::PUBLISH_SCHEMA )) return "Permission denied.";
        
        Schema updateSchema = *context.mSchemaHandle.getSchema ();

        if ( updateSchema.hasCollisions ( this->mSchema )) return "Error publishing schema - found collisions.";
        if ( !updateSchema.compose ( this->mSchema )) return "Error publishing schema.";
        
        context.mLedger.setObject < Schema >( FormatLedgerKey::forSchema (), updateSchema );
        context.mSchemaHandle.reset ( context.mLedger );

        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
