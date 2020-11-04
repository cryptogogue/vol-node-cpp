// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_GENESISSQLITE_H
#define VOLITION_TRANSACTIONS_GENESISSQLITE_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/MinerInfo.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// RestoreAccount
//================================================================//
class RestoreAccount :
    public AbstractSerializable {
public:

    string                              mName;
    u64                                 mBalance;
    SerializableSharedPtr < Policy >    mPolicy;
    SerializableSharedPtr < Policy >    mBequest;

    SerializableMap < string, KeyAndPolicy >                    mKeys;
    SerializableList < SerializableSharedConstPtr < Asset >>    mInventory;

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom          ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo            ( AbstractSerializerTo& serializer ) const override;
    
    //----------------------------------------------------------------//
    TransactionResult       apply                                       ( Ledger& ledger ) const;
};

//================================================================//
// GenesisSQLite
//================================================================//
class GenesisSQLite :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "GENESIS_SQLITE" )
    TRANSACTION_WEIGHT ( 0 )
    TRANSACTION_MATURITY ( 0 )

    SerializableList < RestoreAccount >     mAccounts;

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;
    TransactionResult       AbstractTransactionBody_apply           ( TransactionContext& context ) const override;
    TransactionResult       AbstractTransactionBody_genesis         ( Ledger& ledger ) const override;
    
    //----------------------------------------------------------------//
    void                    load                    ( string filename );
};

} // namespace Transactions
} // namespace Volition
#endif
