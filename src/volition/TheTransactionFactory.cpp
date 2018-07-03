// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "TheTransactionFactory.h"

#include "transactions/AccountPolicy.h"
#include "transactions/KeyPolicy.h"
#include "transactions/OpenAccount.h"
#include "transactions/RegisterMiner.h"
#include "transactions/TransferFunds.h"

namespace Volition {

//================================================================//
// TheTransactionFactory
//================================================================//

//----------------------------------------------------------------//
AbstractTransaction* TheTransactionFactory::create ( const Poco::JSON::Object& object ) const {

    string typeString = object.optValue < string >( "type", "" );

    AbstractTransaction* transaction = this->Factory::create ( typeString );
    assert ( transaction );
    if ( transaction ) {
        transaction->fromJSON ( object );
    }
    return transaction;
}

//----------------------------------------------------------------//
TheTransactionFactory::TheTransactionFactory () {

    this->registerTransaction < Volition::Transaction::AccountPolicy >();
    this->registerTransaction < Volition::Transaction::KeyPolicy >();
    this->registerTransaction < Volition::Transaction::OpenAccount >();
    this->registerTransaction < Volition::Transaction::RegisterMiner >();
    this->registerTransaction < Volition::Transaction::TransferFunds >();
}

//----------------------------------------------------------------//
TheTransactionFactory::~TheTransactionFactory () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
