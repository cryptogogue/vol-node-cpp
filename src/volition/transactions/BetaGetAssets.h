// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_BETA_GET_ASSETS_H
#define VOLITION_TRANSACTIONS_BETA_GET_ASSETS_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Schema.h>

namespace Volition {
namespace Transactions {

//================================================================//
// BetaGetAssets
//================================================================//
class BetaGetAssets :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "BETA_GET_ASSETS" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    u64         mNumAssets;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "schema",        this->mNumAssets );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "schema",        this->mNumAssets );
    }

    //----------------------------------------------------------------//
    bool AbstractTransactionBody_apply ( Ledger& ledger ) const override {
        
        Schema::Index schemaCount = ledger.getSchemaCount ();
        for ( Schema::Index i = 0; i < schemaCount; ++i ) {
            shared_ptr < Schema > schema = legder.getSchema ( i );
            
        }
        
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
