// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractTransactionBody.h>
#include <volition/Transaction.h>

namespace Volition {

//================================================================//
// AbstractTransactionBody
//================================================================//

//----------------------------------------------------------------//
AbstractTransactionBody::AbstractTransactionBody () {
}

//----------------------------------------------------------------//
AbstractTransactionBody::~AbstractTransactionBody () {
}

//----------------------------------------------------------------//
bool AbstractTransactionBody::apply ( Ledger& ledger ) const {

    return this->AbstractTransactionBody_apply ( ledger );
}

//----------------------------------------------------------------//
u64 AbstractTransactionBody::maturity () const {
    return this->AbstractTransactionBody_maturity ();
}

//----------------------------------------------------------------//
string AbstractTransactionBody::typeString () const {
    return this->AbstractTransactionBody_typeString ();
}

//----------------------------------------------------------------//
u64 AbstractTransactionBody::weight () const {
    return this->AbstractTransactionBody_weight ();
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void AbstractTransactionBody::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    string type;
    
    serializer.serialize ( "type",      type );
    serializer.serialize ( "maker",     this->mMaker );
    
    assert ( type == this->typeString ());
}

//----------------------------------------------------------------//
void AbstractTransactionBody::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "type",      this->typeString ());
    serializer.serialize ( "maker",     this->mMaker );
}

} // namespace Volition
