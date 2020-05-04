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
TransactionResult AbstractTransactionBody::apply ( TransactionContext& context ) const {
    return this->AbstractTransactionBody_apply ( context );
}

//----------------------------------------------------------------//
u64 AbstractTransactionBody::cost () const {
    return this->gratuity () + this->AbstractTransactionBody_cost ();
}

//----------------------------------------------------------------//
u64 AbstractTransactionBody::gratuity () const {
    return this->mMaker ? this->mMaker->getGratuity () : 0;
}

//----------------------------------------------------------------//
u64 AbstractTransactionBody::maturity () const {
    return this->AbstractTransactionBody_maturity ();
}

//----------------------------------------------------------------//
string AbstractTransactionBody::note () const {
    return this->mNote;
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
    serializer.serialize ( "note",      this->mNote );
    
    assert ( type == this->typeString ());
}

//----------------------------------------------------------------//
void AbstractTransactionBody::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "type",      this->typeString ());
    serializer.serialize ( "maker",     this->mMaker );
    serializer.serialize ( "note",      this->mNote );
}

//----------------------------------------------------------------//
u64 AbstractTransactionBody::AbstractTransactionBody_cost () const {

    return 0;
}

} // namespace Volition
