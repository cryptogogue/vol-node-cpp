// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_REGISTERMINER_H
#define VOLITION_TRANSACTIONS_REGISTERMINER_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/MinerInfo.h>

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

    string                                      mAccountName;
    SerializableSharedConstPtr < MinerInfo >    mMinerInfo;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "minerInfo",     this->mMinerInfo );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "minerInfo",     this->mMinerInfo );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
    
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::REGISTER_MINER )) return "Permission denied.";
        if ( !this->mMinerInfo ) return "Missing miner info.";
        if ( !this->mMinerInfo->getPublicKey ()) return "Missing miner public key.";
        if ( !this->mMinerInfo->getVisage ()) return "Missing miner visage.";
        
        return context.mLedger.registerMiner ( context.mLedger.getAccountID ( this->mAccountName ), *this->mMinerInfo );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
