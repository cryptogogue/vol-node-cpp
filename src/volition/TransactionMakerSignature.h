// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONMAKERSIGNATURE_H
#define VOLITION_TRANSACTIONMAKERSIGNATURE_H

#include <volition/common.h>
#include <volition/Signature.h>

namespace Volition {

//================================================================//
// TransactionMakerSignature
//================================================================//
class TransactionMakerSignature :
    public Signature {
protected:

    u64                     mGratuity;
    string                  mAccountName;   // account of signer; account being modified; sender account
    string                  mKeyName;       // will identify a key to an account that can pay
    u64                     mNonce;

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serialize      ( AbstractSerializer& serializer ) override;

public:

    //----------------------------------------------------------------//
                            TransactionMakerSignature           ();
                            ~TransactionMakerSignature          ();
};

} // namespace Volition
#endif
