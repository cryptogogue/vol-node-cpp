// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTION_ACCOUNTPOLICY_H
#define VOLITION_TRANSACTION_ACCOUNTPOLICY_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transaction {

//================================================================//
// AccountPolicy
//================================================================//
class AccountPolicy :
    public AbstractTransaction {
private:

    string                                  mAccountName;
    string                                  mPolicyName;
    unique_ptr < Policy >                   mPolicy;

    //----------------------------------------------------------------//
    void                    AbstractHashable_hash               ( Poco::DigestOutputStream& digestStream ) const override;
    void                    AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                    AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;
    void                    AbstractTransaction_apply           ( State& state ) const override;

public:

    TRANSACTION_TYPE ( "ACCOUNT_POLICY" )
    TRANSACTION_WEIGHT ( 1 )

    //----------------------------------------------------------------//
                            AccountPolicy           ();
                            ~AccountPolicy          ();
};

} // namespace Transaction
} // namespace Volition
#endif
