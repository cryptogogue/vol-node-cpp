// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_PUBLISH_SCHEMA_AND_RESET_H
#define VOLITION_TRANSACTIONS_PUBLISH_SCHEMA_AND_RESET_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Digest.h>
#include <volition/Miner.h>
#include <volition/Schema.h>

namespace Volition {
namespace Transactions {

//================================================================//
// PublishSchemaAndReset
//================================================================//
class PublishSchemaAndReset :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "PUBLISH_SCHEMA_AND_RESET" )
    TRANSACTION_WEIGHT ( 0 )
    TRANSACTION_MATURITY ( 0 )

    Schema      mSchema;
    string      mDeckName;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "schema",        this->mSchema );
        serializer.serialize ( "deckName",      this->mDeckName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "schema",        this->mSchema );
        serializer.serialize ( "deckName",      this->mDeckName );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::PUBLISH_SCHEMA_AND_RESET )) return "Permission denied.";

        LedgerResult result = context.mLedger.checkSchemaMethods ( this->mSchema );
        if ( !result ) return result;

        context.mLedger.setSchema ( this->mSchema );
        context.mSchemaHandle.reset ( context.mLedger );

        if ( this->mDeckName.size () > 0 ) {
            return context.mLedger.awardDeck ( *context.mSchemaHandle, context.mIndex, this->mDeckName, context.mTime );
        }
        return true;
    }
    
    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_control ( Miner& miner ) const override {
        
        printf ( "CONTROL: doing publish and reset.\n" );
        
        miner.reset ();
        return true;
    }
    
    //----------------------------------------------------------------//
    bool AbstractTransactionBody_needsControl () const override {
        
        return true;
    }

    //----------------------------------------------------------------//
    u64 AbstractTransactionBody_nonce () const override {
    
        return 0;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
