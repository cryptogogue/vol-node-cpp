// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractControlCommandBody.h>
#include <volition/Miner.h>
#include <volition/Transaction.h>

namespace Volition {

//================================================================//
// AbstractControlCommandBody
//================================================================//

//----------------------------------------------------------------//
AbstractControlCommandBody::AbstractControlCommandBody () {
}

//----------------------------------------------------------------//
AbstractControlCommandBody::~AbstractControlCommandBody () {
}

//----------------------------------------------------------------//
LedgerResult AbstractControlCommandBody::control ( Miner& miner ) const {
    return this->AbstractControlCommandBody_execute ( miner );
}

//----------------------------------------------------------------//
Miner::Control AbstractControlCommandBody::controlLevel () const {
    return this->AbstractControlCommandBody_controlLevel ();
}

//----------------------------------------------------------------//
string AbstractControlCommandBody::typeString () const {
    return this->AbstractControlCommandBody_typeString ();
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void AbstractControlCommandBody::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    string type;
    
    serializer.serialize ( "type",      type );

    assert ( type == this->typeString ());
}

//----------------------------------------------------------------//
void AbstractControlCommandBody::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "type",      this->typeString ());
}

//----------------------------------------------------------------//
LedgerResult AbstractControlCommandBody::AbstractControlCommandBody_execute ( Miner& miner  ) const {
    UNUSED ( miner );
    
    return true;
}

//----------------------------------------------------------------//
Miner::Control AbstractControlCommandBody::AbstractControlCommandBody_controlLevel () const {

    return Miner::CONTROL_NONE;
}

} // namespace Volition
