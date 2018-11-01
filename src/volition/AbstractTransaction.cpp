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

    if (( ledger.getVersion () == 0 ) || ( this->AbstractTransaction_checkSignature ( ledger ))) {
        if ( this->AbstractTransaction_apply ( ledger )) {
            this->AbstractTransaction_incrementNonce ( ledger );
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------//
string AbstractTransaction::typeString () const {
    return this->AbstractTransaction_typeString ();
}

//----------------------------------------------------------------//
size_t AbstractTransaction::weight () const {
    return this->AbstractTransaction_weight ();
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
