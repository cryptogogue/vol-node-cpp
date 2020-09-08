// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_REGISTER_MINER_H
#define VOLITION_TRANSACTIONS_REGISTER_MINER_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// RegisterMiner
//================================================================//
class RegisterMiner :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "REGISTER_MINER" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string          mURL;
    string          mMotto;
    Signature       mVisage;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "url",           this->mURL );
        serializer.serialize ( "motto",         this->mMotto );
        serializer.serialize ( "visage",        this->mVisage );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "url",           this->mURL );
        serializer.serialize ( "motto",         this->mMotto );
        serializer.serialize ( "visage",        this->mVisage );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
    
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::REGISTER_MINER )) return "Permission denied.";
    
        return context.mLedger.registerMiner ( context.mAccount.mIndex, this->mMaker->getKeyName (), this->mURL, this->mMotto, this->mVisage );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
