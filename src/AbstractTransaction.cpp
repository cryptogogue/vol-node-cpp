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
AbstractTransaction::AbstractTransaction () {
}

//----------------------------------------------------------------//
AbstractTransaction::~AbstractTransaction () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void AbstractTransaction::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {
}

//----------------------------------------------------------------//
void AbstractTransaction::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {
}

//----------------------------------------------------------------//
void AbstractTransaction::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
}

} // namespace Volition
