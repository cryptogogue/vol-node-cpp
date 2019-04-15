// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_INVOKESCHEMAMETHOD_H
#define VOLITION_TRANSACTIONS_INVOKESCHEMAMETHOD_H

#include <volition/common.h>
#include <volition/AbstractSingleSignerTransaction.h>
#include <volition/SchemaLua.h>

namespace Volition {
namespace Transactions {

//================================================================//
// InvokeSchemaMethod
//================================================================//
class InvokeSchemaMethod :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "INVOKE_SCHEMA_METHOD" )
    
    string                                  mSchemaName;
    string                                  mMethodName;
    u64                                     mWeight;
    u64                                     mMaturity;
    SerializableVector < AssetIdentifier >  mAssetIdentifiers;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "schemaName",            this->mSchemaName );
        serializer.serialize ( "methodName",            this->mMethodName );
        serializer.serialize ( "weight",                this->mWeight );
        serializer.serialize ( "maturity",              this->mMaturity );
        serializer.serialize ( "assetIdentifiers",      this->mAssetIdentifiers  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "schemaName",            this->mSchemaName );
        serializer.serialize ( "methodName",            this->mMethodName );
        serializer.serialize ( "weight",                this->mWeight );
        serializer.serialize ( "maturity",              this->mMaturity );
        serializer.serialize ( "assetIdentifiers",      this->mAssetIdentifiers  );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
        
        shared_ptr < Schema > schema = ledger.getSchema ( this->mSchemaName );
        if ( !schema ) return false;
        
        // TODO: this is brutally inefficient, but we're doing it for now. can add a cache of SchemaLua objects later to speed things up.
        SchemaLua schemaLua ( *schema );
        return schemaLua.runMethod ( ledger, this->mMethodName, this->mWeight, this->mMaturity, this->mAssetIdentifiers.data (), this->mAssetIdentifiers.size ());
    }
    
    //----------------------------------------------------------------//
    u64 AbstractTransaction_maturity () const override {
    
        return this->mMaturity;
    }
    
    //----------------------------------------------------------------//
    u64 AbstractTransaction_weight () const override {
    
        return this->mWeight;
    }
    
    //----------------------------------------------------------------//
    bool AbstractTransaction_verify ( const Ledger& ledger ) const override {
        
        if ( AbstractSingleSignerTransaction::AbstractTransaction_verify ( ledger )) {
        
            shared_ptr < Schema > schema = ledger.getSchema ( this->mSchemaName );
            if ( schema ) {
                return schema->verifyMethod ( this->mMethodName, this->mWeight, this->mMaturity );
            }
        }
        return false;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
