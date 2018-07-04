// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTION_KEYPOLICY_H
#define VOLITION_TRANSACTION_KEYPOLICY_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transaction {

//================================================================//
// KeyPolicy
//================================================================//
class KeyPolicy :
    public AbstractTransaction {
private:

    string                                  mKeyName;
    unique_ptr < Poco::Crypto::ECKey >      mKey;
    string                                  mPolicyName;
    unique_ptr < Policy >                   mPolicy;

    //----------------------------------------------------------------//
    void                    AbstractHashable_hash               ( Poco::DigestOutputStream& digestStream ) const override;
    void                    AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                    AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;
    void                    AbstractTransaction_apply           ( State& state ) const override;

public:

    TRANSACTION_TYPE ( "KEY_POLICY" )
    TRANSACTION_WEIGHT ( 1 )

    //----------------------------------------------------------------//
                            KeyPolicy               ();
                            ~KeyPolicy              ();
};

} // namespace Transaction
} // namespace Volition
#endif
