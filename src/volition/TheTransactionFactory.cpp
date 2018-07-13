// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/TheTransactionFactory.h>
#include <volition/Transactions.h>

namespace Volition {

//================================================================//
// TheTransactionFactory
//================================================================//

//----------------------------------------------------------------//
unique_ptr < AbstractTransaction > TheTransactionFactory::create ( string typeString ) const {

    return this->Factory::create ( typeString );
}

//----------------------------------------------------------------//
TheTransactionFactory::TheTransactionFactory () {

    this->registerTransaction < Volition::Transaction::AccountPolicy >();
    this->registerTransaction < Volition::Transaction::GenesisMiner >();
    this->registerTransaction < Volition::Transaction::KeyPolicy >();
    this->registerTransaction < Volition::Transaction::OpenAccount >();
    this->registerTransaction < Volition::Transaction::RegisterMiner >();
    this->registerTransaction < Volition::Transaction::SendVOL >();
}

//----------------------------------------------------------------//
TheTransactionFactory::~TheTransactionFactory () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
