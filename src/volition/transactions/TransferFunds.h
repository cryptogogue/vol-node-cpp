// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTION_TRANSFERFUNDS_H
#define VOLITION_TRANSACTION_TRANSFERFUNDS_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>

namespace Volition {
namespace Transaction {

//================================================================//
// TransferFunds
//================================================================//
class TransferFunds :
    public AbstractTransaction {
private:

    string                  mAccountName;
    u64                     mAmount;

    //----------------------------------------------------------------//
    void                    AbstractHashable_hash               ( Poco::DigestOutputStream& digestStream ) const override;
    void                    AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                    AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;
    void                    AbstractTransaction_apply           ( State& state ) const override;

public:

    TRANSACTION_TYPE ( "TRANSFER_FUNDS" )
    TRANSACTION_WEIGHT ( 1 )

    //----------------------------------------------------------------//
                            TransferFunds               ();
                            ~TransferFunds              ();
};

} // namespace Transaction
} // namespace Volition
#endif
