//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

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
