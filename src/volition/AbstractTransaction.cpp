// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <AbstractTransaction.h>
#include <Hash.h>
#include <Serialize.h>

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
void AbstractTransaction::apply ( State& state ) const {
    this->AbstractTransaction_apply ( state );
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
void AbstractTransaction::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {

    digestStream << this->typeString ();
    Hash::hashOrNull ( digestStream, this->mMakerSignature.get ());
}

//----------------------------------------------------------------//
void AbstractTransaction::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {

    assert ( this->typeString () == object.getValue < string >( "type" ));
    
    this->mMakerSignature = Serialize::getSerializableFromJSON < TransactionMakerSignature >( object, "makerSignature" );
}

//----------------------------------------------------------------//
void AbstractTransaction::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {

    object.set ( "type",            this->typeString ().c_str ());
    
    Serialize::setSerializableToJSON ( object, "makerSignature", this->mMakerSignature.get ());
}

} // namespace Volition
