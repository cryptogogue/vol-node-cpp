// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTION_REGISTERKEY_H
#define VOLITION_TRANSACTION_REGISTERKEY_H

#include "common.h"

#include "AbstractTransaction.h"

namespace Volition {
namespace Transaction {

//================================================================//
// RegisterKey
//================================================================//
class RegisterKey :
    public AbstractTransaction {
private:

    unique_ptr < Poco::Crypto::ECKey >      mPublicKey;
    string                                  mKeyName;

    //----------------------------------------------------------------//
    void                    AbstractHashable_hash               ( Poco::DigestOutputStream& digestStream ) const override;
    void                    AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                    AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;
    void                    AbstractTransaction_apply           ( State& state ) const override;

public:

    TRANSACTION_TYPE ( "REGISTER_KEY" )
    TRANSACTION_WEIGHT ( 1 )

    //----------------------------------------------------------------//
                            RegisterKey             ();
                            ~RegisterKey            ();
};

} // namespace Transaction
} // namespace Volition
#endif
