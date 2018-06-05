// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "TheTransactionFactory.h"

namespace Volition {

//================================================================//
// TheTransactionFactory
//================================================================//

//----------------------------------------------------------------//
AbstractTransaction* TheTransactionFactory::create ( const Poco::JSON::Object& object ) const {

    string typeString = object.optValue < string >( "type", "" );

    AbstractTransaction* transaction = this->Factory::create ( typeString );
    if ( transaction ) {
        transaction->fromJSON ( object );
    }
    return transaction;
}

//----------------------------------------------------------------//
TheTransactionFactory::TheTransactionFactory () {
}

//----------------------------------------------------------------//
TheTransactionFactory::~TheTransactionFactory () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
