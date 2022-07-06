// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_PUBLISHSCHEMA_H
#define VOLITION_TRANSACTIONS_PUBLISHSCHEMA_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/Digest.h>
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
    TRANSACTION_MATURITY ( 0 )

    Schema      mSchema;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "schema",        this->mSchema );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "schema",        this->mSchema );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::PUBLISH_SCHEMA )) return "Permission denied.";
        
        Schema updateSchema = context.mLedger.getSchema ();

        const SchemaVersion& version0 = updateSchema.getVersion ();
        const SchemaVersion& version1 = this->mSchema.getVersion ();

        if ( !(( version0.mRelease.size () || version1.mRelease.size ()))) return "Error publishing schema - missing release name.";
        if ( !version0.checkNext ( version1 )) return "Error publishing schema - version must increase.";
        if ( updateSchema.hasCollisions ( this->mSchema )) return "Error publishing schema - found collisions.";
        if ( !updateSchema.compose ( this->mSchema )) return "Error publishing schema.";
        
        LedgerResult result = context.mLedger.checkSchemaMethodsAndRewards ( this->mSchema );
        if ( !result ) return result;
        
        context.mLedger.setSchema ( updateSchema );

        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
