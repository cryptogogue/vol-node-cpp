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
void TransactionMakerSignature::AbstractSerializable_serialize ( AbstractSerializer& serializer ) {

    serializer.serialize ( "gratuity",      this->mGratuity );
    serializer.serialize ( "accountName",   this->mAccountName );
    serializer.serialize ( "keyName",       this->mKeyName );
    serializer.serialize ( "nonce",         this->mNonce );
}

} // namespace Volition
