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

        LedgerResult result = context.mLedger.checkSchemaMethodsAndRewards ( this->mSchema );
        if ( !result ) return result;

        context.mLedger.setSchema ( this->mSchema );

        AccountID::Index totalAccounts = context.mLedger.getValue < AccountID::Index >( Ledger::keyFor_globalAccountCount ());
        for ( AccountID::Index i = 0; i < totalAccounts; ++i ) {
            context.mLedger.clearInventory ( AccountID ( i ));
        }

        if ( this->mDeckName.size () > 0 ) {
            return context.mLedger.awardDeck ( context.mAccountID, this->mDeckName, context.mTime );
        }
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
