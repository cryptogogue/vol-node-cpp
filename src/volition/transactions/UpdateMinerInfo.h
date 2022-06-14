// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_UPDATEMINERINFO_H
#define VOLITION_TRANSACTIONS_UPDATEMINERINFO_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/MinerInfo.h>

namespace Volition {
namespace Transactions {

//================================================================//
// UpdateMinerInfo
//================================================================//
class UpdateMinerInfo :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "UPDATE_MINER_INFO" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    SerializableSharedConstPtr < MinerInfo >    mMinerInfo;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "minerInfo",     this->mMinerInfo );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "minerInfo",     this->mMinerInfo );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
    
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::UPDATE_MINER_INFO )) return "Permission denied.";
        if ( !this->mMinerInfo ) return "Missing miner info.";
        
        if ( context.mAccountODBM.isMiner ()) {
            return context.mLedger.updateMinerInfo ( context.mAccountID, *this->mMinerInfo );
        }
        return context.mLedger.registerMiner ( context.mAccountID, *this->mMinerInfo, true );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
