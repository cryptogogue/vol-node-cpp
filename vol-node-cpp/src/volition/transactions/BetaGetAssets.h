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
        
        serializer.serialize ( "numAssets",     this->mNumAssets );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "numAssets",     this->mNumAssets );
    }

    //----------------------------------------------------------------//
    bool AbstractTransactionBody_apply ( Ledger& ledger, SchemaHandle& schemaHandle ) const override {
        
        const Schema::Definitions& definitions = schemaHandle->getDefinitions ();
        
        Schema::Definitions::const_iterator definitionIt = definitions.cbegin ();
        for ( ; definitionIt != definitions.cend (); ++definitionIt ) {
        
            ledger.awardAsset ( *schemaHandle, this->mMaker->getAccountName (), definitionIt->first, ( int )this->mNumAssets );
        }
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
