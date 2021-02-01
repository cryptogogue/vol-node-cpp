// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_TRANSFORMASSETS_H
#define VOLITION_TRANSACTIONS_TRANSFORMASSETS_H

#include <volition/common.h>
#include <volition/AbstractSingleSignerTransaction.h>
#include <volition/SchemaLua.h>

namespace Volition {
namespace Transactions {

//================================================================//
// TransformAssets
//================================================================//
class InvokeSchemaMethod :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "TRANSFORM_ASSETS" )

    size_t                                  mWeight;
    size_t                                  mMaturity;

    string                                  mSchemaName;
    string                                  mMethodName;
    SerializableVector < AssetIdentifier >  mAssetIdentifiers;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "weight",                this->mWeight );
        serializer.serialize ( "maturity",              this->mMaturity );
        serializer.serialize ( "schemaName",            this->mSchemaName );
        serializer.serialize ( "methodName",            this->mMethodName );
        serializer.serialize ( "assetIdentifiers",      this->mAssetIdentifiers  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "weight",                this->mWeight );
        serializer.serialize ( "maturity",              this->mMaturity );
        serializer.serialize ( "schemaName",            this->mSchemaName );
        serializer.serialize ( "methodName",            this->mMethodName );
        serializer.serialize ( "assetIdentifiers",      this->mAssetIdentifiers  );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
        
        VersionedValue < Schema > schema = ledger.getSchema ( this->mSchemaName );
        if ( !schema ) return false;
        
        // TODO: this is brutally inefficient, but we're doing it for now. can add a cache of SchemaLua objects later to speed things up.
        SchemaLua schemaLua ( *schema );
        return schemaLua.runMethod ( ledger, this->mMethodName, this->mWeight, this->mMaturity, this->mAssetIdentifiers.data (), this->mAssetIdentifiers.size ());
        
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
