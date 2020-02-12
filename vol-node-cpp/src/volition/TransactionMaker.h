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
    string                  mAccountName;   // account of signer; account being modified; sender account
    string                  mKeyName;
    u64                     mNonce;

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom  ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo    ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
    string                  getAccountName                      () const;
    u64                     getGratuity                         () const;
    string                  getKeyName                          () const;
    u64                     getNonce                            () const;
                            TransactionMaker                    ();
                            ~TransactionMaker                   ();
};

} // namespace Volition
#endif
