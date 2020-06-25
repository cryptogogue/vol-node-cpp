// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONRESULT_H
#define VOLITION_TRANSACTIONRESULT_H

#include <volition/common.h>
#include <volition/LedgerResult.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

class Transaction;

//================================================================//
// TransactionResult
//================================================================//
class TransactionResult :
    public LedgerResult {
private:

    string      mUUID;

public:

    //----------------------------------------------------------------//
    string      getUUID                     () const;
    void        setTransactionDetails       ( const Transaction& transaction );
                TransactionResult           ( bool status );
                TransactionResult           ( const char* message );
                TransactionResult           ( string message );
                TransactionResult           ( const LedgerResult& ledgerResult );
};

} // namespace Volition
#endif
