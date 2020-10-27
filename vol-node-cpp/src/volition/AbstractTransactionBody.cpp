// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractTransactionBody.h>
#include <volition/Miner.h>
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
u64 AbstractTransactionBody::nonce () const {
    return this->mMaker ? this->mMaker->getNonce () : 0;
}

//----------------------------------------------------------------//
void AbstractTransactionBody::setMaker ( const TransactionMaker& maker ) {
    this->mMaker = make_unique < TransactionMaker >( maker );
}

//----------------------------------------------------------------//
void AbstractTransactionBody::setMaxHeight ( u64 maxHeight ) {
    this->mMaxHeight = maxHeight;
}

//----------------------------------------------------------------//
void AbstractTransactionBody::setRecordBy ( time_t recordBy ) {
    this->mRecordBy = recordBy;
}

//----------------------------------------------------------------//
void AbstractTransactionBody::setUUID ( string uuid ) {
    this->mUUID = uuid;
}

//----------------------------------------------------------------//
string AbstractTransactionBody::typeString () const {
    return this->AbstractTransactionBody_typeString ();
}

//----------------------------------------------------------------//
string AbstractTransactionBody::uuid () const {
    return this->mUUID;
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
    serializer.serialize ( "maxHeight", this->mMaxHeight );
    serializer.serialize ( "recordBy",  this->mRecordBy );
    serializer.serialize ( "uuid",      this->mUUID );
    
    assert ( type == this->typeString ());
}

//----------------------------------------------------------------//
void AbstractTransactionBody::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "type",      this->typeString ());
    serializer.serialize ( "maker",     this->mMaker );
    serializer.serialize ( "maxHeight", this->mMaxHeight );
    serializer.serialize ( "recordBy",  this->mRecordBy );
    serializer.serialize ( "uuid",      this->mUUID );
}

//----------------------------------------------------------------//
u64 AbstractTransactionBody::AbstractTransactionBody_cost () const {

    return 0;
}

} // namespace Volition
