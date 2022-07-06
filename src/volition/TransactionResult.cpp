// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractTransaction.h>
#include <volition/TransactionEnvelope.h>
#include <volition/TransactionResult.h>

namespace Volition {

//================================================================//
// TransactionResult
//================================================================//

//----------------------------------------------------------------//
string TransactionResult::getUUID () const {
    return this->mUUID;
}

//----------------------------------------------------------------//
void TransactionResult::setTransactionDetails ( const TransactionEnvelope& transaction ) {
    this->mUUID = transaction.getUUID ();
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
