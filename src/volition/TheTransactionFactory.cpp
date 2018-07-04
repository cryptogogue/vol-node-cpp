// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/TheTransactionFactory.h>

#include <volition/transactions/AccountPolicy.h>
#include <volition/transactions/KeyPolicy.h>
#include <volition/transactions/OpenAccount.h>
#include <volition/transactions/RegisterMiner.h>
#include <volition/transactions/TransferFunds.h>

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
