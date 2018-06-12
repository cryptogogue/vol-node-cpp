// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "TheTransactionFactory.h"
#include "transactions/RegisterMiner.h"

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

    this->registerTransaction < Volition::Transaction::RegisterMiner >();
}

//----------------------------------------------------------------//
TheTransactionFactory::~TheTransactionFactory () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
