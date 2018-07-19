// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/TransactionMakerSignature.h>

namespace Volition {

//================================================================//
// TransactionMakerSignature
//================================================================//

//----------------------------------------------------------------//
TransactionMakerSignature::TransactionMakerSignature () :
    mGratuity ( 0 ),
    mNonce ( 0 ) {
}

//----------------------------------------------------------------//
TransactionMakerSignature::~TransactionMakerSignature () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
string TransactionMakerSignature::getAccountName () const {

    return this->mAccountName;
}

//----------------------------------------------------------------//
u64 TransactionMakerSignature::getGratuity () const {

    return this->mGratuity;
}

//----------------------------------------------------------------//
string TransactionMakerSignature::getKeyName () const {

    return this->mKeyName;
}

//----------------------------------------------------------------//
u64 TransactionMakerSignature::getNonce () const {

    return this->mNonce;
}

//----------------------------------------------------------------//
void TransactionMakerSignature::AbstractSerializable_serialize ( AbstractSerializer& serializer ) {

    serializer.serialize ( "accountName",   this->mAccountName );
    serializer.serialize ( "gratuity",      this->mGratuity );
    serializer.serialize ( "keyName",       this->mKeyName );
    serializer.serialize ( "nonce",         this->mNonce );
}

} // namespace Volition
