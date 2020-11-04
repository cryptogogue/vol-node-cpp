// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_LOADLEDGER_H
#define VOLITION_TRANSACTIONS_LOADLEDGER_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/MinerInfo.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// LoadLedgerAccount
//================================================================//
class LoadLedgerAccount :
    public AbstractSerializable {
public:

    string                                      mName;
    u64                                         mBalance;
    Policy                                      mPolicy;
    SerializableSharedPtr < Policy >            mBequest;
    SerializableSharedConstPtr < MinerInfo >    mMinerInfo;

    SerializableMap < string, KeyAndPolicy >                    mKeys;
    SerializableList < SerializableSharedConstPtr < Asset >>    mInventory;

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom          ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo            ( AbstractSerializerTo& serializer ) const override;
    
    //----------------------------------------------------------------//
    TransactionResult       apply                                       ( Ledger& ledger ) const;
};

//================================================================//
// LoadLedger
//================================================================//
class LoadLedger :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "LOAD_LEDGER" )
    TRANSACTION_WEIGHT ( 0 )
    TRANSACTION_MATURITY ( 0 )

    string                                      mIdentity;
    Schema                                      mSchema;
    SerializableList < LoadLedgerAccount >      mAccounts;

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;
    TransactionResult       AbstractTransactionBody_apply           ( TransactionContext& context ) const override;
    TransactionResult       AbstractTransactionBody_genesis         ( Ledger& ledger ) const override;
    
    //----------------------------------------------------------------//
    void                    init                    ( Ledger& ledger );
};

} // namespace Transactions
} // namespace Volition
#endif
