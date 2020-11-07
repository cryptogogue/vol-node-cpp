// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Ledger.h>
#include <volition/TransactionMaker.h>

namespace Volition {

//================================================================//
// TransactionMaker
//================================================================//

//----------------------------------------------------------------//
TransactionMaker::TransactionMaker () :
    mGratuity ( 0 ),
    mProfitShare ( 0 ),
    mTransferTax ( 0 ),
    mKeyName ( Ledger::MASTER_KEY_NAME ),
    mNonce ( 0 ) {
}

//----------------------------------------------------------------//
TransactionMaker::~TransactionMaker () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void TransactionMaker::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    serializer.serialize ( "accountName",   this->mAccountName );
    serializer.serialize ( "gratuity",      this->mGratuity );
    serializer.serialize ( "profitShare",   this->mProfitShare );
    serializer.serialize ( "transferTax",   this->mTransferTax );
    serializer.serialize ( "keyName",       this->mKeyName );
    serializer.serialize ( "nonce",         this->mNonce );
}

//----------------------------------------------------------------//
void TransactionMaker::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "accountName",   this->mAccountName );
    serializer.serialize ( "gratuity",      this->mGratuity );
    serializer.serialize ( "profitShare",   this->mProfitShare );
    serializer.serialize ( "transferTax",   this->mTransferTax );
    serializer.serialize ( "keyName",       this->mKeyName );
    serializer.serialize ( "nonce",         this->mNonce );
}

} // namespace Volition
