// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTION_TRANSFERFUNDS_H
#define VOLITION_TRANSACTION_TRANSFERFUNDS_H

#include "common.h"

#include "AbstractTransaction.h"

namespace Volition {
namespace Transaction {

//================================================================//
// TransferFunds
//================================================================//
class TransferFunds :
    public AbstractTransaction {
private:

    string                  mFromKey;
    string                  mToKey;
    u64                     mAmount;

    //----------------------------------------------------------------//
    void                    AbstractHashable_hash               ( Poco::DigestOutputStream& digestStream ) const override;
    void                    AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                    AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;

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
