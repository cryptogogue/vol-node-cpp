// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_BETA_GET_DECK_H
#define VOLITION_TRANSACTIONS_BETA_GET_DECK_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Schema.h>

namespace Volition {
namespace Transactions {

//================================================================//
// BetaGetDeck
//================================================================//
class BetaGetDeck :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "BETA_GET_DECK" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string      mDeckName;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "deckName",      this->mDeckName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "deckName",      this->mDeckName );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        AbstractLedger& ledger = context.mLedger;
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::BETA_GET_DECK )) return "Permission denied.";
        
        const Schema& schema = context.mLedger.getSchema ();
        const Schema::Deck* deck = schema.getDeck ( this->mDeckName );
        if ( !deck ) return "Deck not found.";
        
        size_t addedAssetCount = 0;
        Schema::Deck::const_iterator deckIt = deck->cbegin ();
        for ( ; deckIt != deck->cend (); ++deckIt ) {
            addedAssetCount += deckIt->second;
        }
        
        AccountODBM accountODBM ( ledger, context.mAccountID );
        size_t assetCount = accountODBM.mAssetCount.get ( 0 );
        
        if ( !context.mAccountEntitlements.check ( AccountEntitlements::MAX_ASSETS, assetCount + addedAssetCount )) {
            double max = context.mAccountEntitlements.resolvePathAs < NumericEntitlement >( AccountEntitlements::MAX_ASSETS )->getUpperLimit ().mLimit;
            return Format::write ( "Transaction would overflow account inventory limit of %d assets.", ( int )max );
        }
        
        return ledger.awardDeck ( context.mAccountID, this->mDeckName, context.mTime );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
