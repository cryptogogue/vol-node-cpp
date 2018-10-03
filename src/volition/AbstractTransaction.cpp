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
bool AbstractTransaction::apply ( State& state ) const {

    const TransactionMakerSignature* makerSignature = this->mMakerSignature.get ();

    if (( state.getVersion () == 0 ) || ( makerSignature && state.checkMakerSignature ( makerSignature ))) {
        if ( this->AbstractTransaction_apply ( state )) {
            state.consumeMakerSignature ( makerSignature );
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
    serializer.serialize ( "maker",     this->mMakerSignature );
    
    assert ( type == this->typeString ());
}

//----------------------------------------------------------------//
void AbstractTransaction::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "type",      this->typeString ());
    serializer.serialize ( "maker",     this->mMakerSignature );
}

} // namespace Volition
