// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Transactions.h>

namespace Volition {

//================================================================//
// TheTransactionBodyFactory
//================================================================//

//----------------------------------------------------------------//
unique_ptr < AbstractTransactionBody > TheTransactionBodyFactory::create ( string typeString ) const {

    return this->Factory::create ( typeString );
}

//----------------------------------------------------------------//
TheTransactionBodyFactory::TheTransactionBodyFactory () {

    this->registerTransaction < Volition::Transactions::AccountPolicy >();
    this->registerTransaction < Volition::Transactions::AffirmKey >();
    this->registerTransaction < Volition::Transactions::BetaGetAssets >();
    this->registerTransaction < Volition::Transactions::GenesisBlock >();
    this->registerTransaction < Volition::Transactions::GenesisMiner >();
    this->registerTransaction < Volition::Transactions::KeyPolicy >();
    this->registerTransaction < Volition::Transactions::OpenAccount >();
    this->registerTransaction < Volition::Transactions::PublishSchema >();
    this->registerTransaction < Volition::Transactions::RegisterMiner >();
    this->registerTransaction < Volition::Transactions::RunScript >();
    this->registerTransaction < Volition::Transactions::SendAssets >();
    this->registerTransaction < Volition::Transactions::SendVOL >();
}

//----------------------------------------------------------------//
TheTransactionBodyFactory::~TheTransactionBodyFactory () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
