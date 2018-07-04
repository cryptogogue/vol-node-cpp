// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTION_OPENACCOUNT_H
#define VOLITION_TRANSACTION_OPENACCOUNT_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>

namespace Volition {
namespace Transaction {

//================================================================//
// OpenAccount
//================================================================//
class OpenAccount :
    public AbstractTransaction {
private:

    string                                  mAccountName;
    string                                  mKeyName;
    unique_ptr < Poco::Crypto::ECKey >      mKey;
    u64                                     mAmount;

    //----------------------------------------------------------------//
    void                    AbstractHashable_hash               ( Poco::DigestOutputStream& digestStream ) const override;
    void                    AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                    AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;
    void                    AbstractTransaction_apply           ( State& state ) const override;

public:

    TRANSACTION_TYPE ( "OPEN_ACCOUNT" )
    TRANSACTION_WEIGHT ( 1 )

    //----------------------------------------------------------------//
                            OpenAccount             ();
                            OpenAccount             ( string accountName, string keyName, const Poco::Crypto::ECKey& key, u64 amount );
                            ~OpenAccount            ();
};

} // namespace Transaction
} // namespace Volition
#endif
