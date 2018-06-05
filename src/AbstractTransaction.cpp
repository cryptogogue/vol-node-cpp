//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "AbstractTransaction.h"

namespace Volition {

//================================================================//
// AbstractTransaction
//================================================================//

//----------------------------------------------------------------//
AbstractTransaction::AbstractTransaction () :
    mTransactionID ( 0 ),
    mGratuity ( 0 ) {
}

//----------------------------------------------------------------//
AbstractTransaction::~AbstractTransaction () {
}

//----------------------------------------------------------------//
string AbstractTransaction::typeString () {
    return this->AbstractTransaction_typeString ();
}

//----------------------------------------------------------------//
size_t AbstractTransaction::weight () {
    return this->AbstractTransaction_weight ();
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void AbstractTransaction::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {
}

//----------------------------------------------------------------//
void AbstractTransaction::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {

    assert ( this->typeString () == object.getValue < string >( "type" ));
}

//----------------------------------------------------------------//
void AbstractTransaction::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
}

} // namespace Volition
