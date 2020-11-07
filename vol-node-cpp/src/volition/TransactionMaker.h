// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONMAKER_H
#define VOLITION_TRANSACTIONMAKER_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// TransactionMaker
//================================================================//
class TransactionMaker :
    public AbstractSerializable {
protected:

    u64                     mGratuity;
    u64                     mProfitShare;
    u64                     mTransferTax;
    string                  mAccountName;   // account of signer; account being modified; sender account
    string                  mKeyName;
    u64                     mNonce;

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom  ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo    ( AbstractSerializerTo& serializer ) const override;

public:

    GET_SET ( string,       AccountName,        mAccountName )
    GET_SET ( u64,          Gratuity,           mGratuity )
    GET_SET ( string,       KeyName,            mKeyName )
    GET_SET ( u64,          Nonce,              mNonce )
    GET_SET ( u64,          ProfitShare,        mProfitShare )
    GET_SET ( u64,          TransferTax,        mTransferTax )

    //----------------------------------------------------------------//
                            TransactionMaker                    ();
                            ~TransactionMaker                   ();
};

} // namespace Volition
#endif
