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
const TransactionMaker* AbstractTransactionBody::getMaker () const {
    return this->mMaker ? this->mMaker.get () : NULL;
}

//----------------------------------------------------------------//
TransactionResult AbstractTransactionBody::genesis ( AbstractLedger& ledger ) {
    return this->AbstractTransactionBody_genesis ( ledger );
}

//----------------------------------------------------------------//
void AbstractTransactionBody::setMaker ( const TransactionMaker& maker ) {
    this->mMaker = make_unique < TransactionMaker >( maker );
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
    
    assert ( type == this->getTypeString ());
}

//----------------------------------------------------------------//
void AbstractTransactionBody::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "type",      this->getTypeString ());
    serializer.serialize ( "maker",     this->mMaker );
    serializer.serialize ( "maxHeight", this->mMaxHeight );
    serializer.serialize ( "recordBy",  this->mRecordBy );
    serializer.serialize ( "uuid",      this->mUUID );
}

//----------------------------------------------------------------//
string AbstractTransactionBody::AbstractTransactionBody_feeName () const {
    return "";
}

//----------------------------------------------------------------//
TransactionResult AbstractTransactionBody::AbstractTransactionBody_genesis ( AbstractLedger& ledger ) const {
    UNUSED ( ledger );
    return "Missing transaction maker.";
}

//----------------------------------------------------------------//
u64 AbstractTransactionBody::AbstractTransactionBody_sendVOL () const {
    return 0;
}

} // namespace Volition
