// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/TransactionMaker.h>

namespace Volition {

//================================================================//
// TransactionMaker
//================================================================//

//----------------------------------------------------------------//
TransactionMaker::TransactionMaker () :
    mGratuity ( 0 ),
    mNonce ( 0 ) {
}

//----------------------------------------------------------------//
TransactionMaker::~TransactionMaker () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
string TransactionMaker::getAccountName () const {

    return this->mAccountName;
}

//----------------------------------------------------------------//
u64 TransactionMaker::getGratuity () const {

    return this->mGratuity;
}

//----------------------------------------------------------------//
string TransactionMaker::getKeyName () const {

    return this->mKeyName;
}

//----------------------------------------------------------------//
u64 TransactionMaker::getNonce () const {

    return this->mNonce;
}

//----------------------------------------------------------------//
void TransactionMaker::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    serializer.serialize ( "accountName",   this->mAccountName );
    serializer.serialize ( "gratuity",      this->mGratuity );
    serializer.serialize ( "keyName",       this->mKeyName );
    serializer.serialize ( "nonce",         this->mNonce );
}

//----------------------------------------------------------------//
void TransactionMaker::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "accountName",   this->mAccountName );
    serializer.serialize ( "gratuity",      this->mGratuity );
    serializer.serialize ( "keyName",       this->mKeyName );
    serializer.serialize ( "nonce",         this->mNonce );
}

} // namespace Volition
