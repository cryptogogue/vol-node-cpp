// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractTransactionBody.h>
#include <volition/Transaction.h>
#include <volition/TransactionResult.h>

namespace Volition {

//================================================================//
// TransactionResult
//================================================================//

//----------------------------------------------------------------//
string TransactionResult::getNote () const {
    return this->mNote;
}

//----------------------------------------------------------------//
void TransactionResult::setTransactionDetails ( const Transaction& transaction ) {
    this->mNote = transaction.getNote ();
}

//----------------------------------------------------------------//
TransactionResult::TransactionResult ( bool status ) :
    LedgerResult ( status ) {
}

//----------------------------------------------------------------//
TransactionResult::TransactionResult ( const char* message ) :
    LedgerResult ( message ) {
}

//----------------------------------------------------------------//
TransactionResult::TransactionResult ( string message ) :
    LedgerResult ( message ) {
}

TransactionResult::TransactionResult ( const LedgerResult& ledgerResult ) :
    LedgerResult ( ledgerResult ) {
}

} // namespace Volition
