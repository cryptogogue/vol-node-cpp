// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONMAKERSIGNATURE_H
#define VOLITION_TRANSACTIONMAKERSIGNATURE_H

#include <common.h>
#include <Signature.h>

namespace Volition {

//================================================================//
// TransactionMakerSignature
//================================================================//
class TransactionMakerSignature :
    public AbstractHashable,
    public Signature {
protected:

    u64                     mGratuity;
    string                  mKeyName; // will identify a key to an account that can pay
    u64                     mNonce;

    //----------------------------------------------------------------//
    void                    AbstractHashable_hash               ( Poco::DigestOutputStream& digestStream ) const override;
    void                    AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                    AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;

public:

    //----------------------------------------------------------------//
                            TransactionMakerSignature           ();
                            ~TransactionMakerSignature          ();
};

} // namespace Volition
#endif
