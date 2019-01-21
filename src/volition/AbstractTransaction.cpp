// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractTransaction.h>

namespace Volition {

//================================================================//
// AbstractTransaction
//================================================================//

//----------------------------------------------------------------//
AbstractTransaction::AbstractTransaction () {
}

//----------------------------------------------------------------//
AbstractTransaction::~AbstractTransaction () {
}

//----------------------------------------------------------------//
bool AbstractTransaction::apply ( Ledger& ledger ) const {

    // if the trsnaction has a signer, verify () also checks the nonce that prevents from transactions being applied multiple times.
    if (( ledger.getVersion () == 0 ) || ( this->verify ( ledger ))) {
        if ( this->AbstractTransaction_apply ( ledger )) {
            this->AbstractTransaction_incrementNonce ( ledger );
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------//
u64 AbstractTransaction::maturity () const {
    return this->AbstractTransaction_maturity ();
}

//----------------------------------------------------------------//
string AbstractTransaction::typeString () const {
    return this->AbstractTransaction_typeString ();
}

//----------------------------------------------------------------//
u64 AbstractTransaction::weight () const {
    return this->AbstractTransaction_weight ();
}

//----------------------------------------------------------------//
bool AbstractTransaction::verify ( const Ledger& ledger ) const {
    return this->AbstractTransaction_verify ( ledger );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void AbstractTransaction::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    string type;
    
    serializer.serialize ( "type",      type );
    
    assert ( type == this->typeString ());
}

//----------------------------------------------------------------//
void AbstractTransaction::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "type",      this->typeString ());
}

} // namespace Volition
