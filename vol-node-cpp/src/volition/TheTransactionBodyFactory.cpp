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

    this->registerTransaction < Volition::Transactions::AffirmKey >();
    this->registerTransaction < Volition::Transactions::BetaGetAssets >();
    this->registerTransaction < Volition::Transactions::BetaGetDeck >();
    this->registerTransaction < Volition::Transactions::Genesis >();
    this->registerTransaction < Volition::Transactions::LoadLedger >();
    this->registerTransaction < Volition::Transactions::OpenAccount >();
    this->registerTransaction < Volition::Transactions::PublishSchema >();
    this->registerTransaction < Volition::Transactions::PublishSchemaAndReset >();
    this->registerTransaction < Volition::Transactions::RegisterMiner >();
    this->registerTransaction < Volition::Transactions::RenameAccount >();
    this->registerTransaction < Volition::Transactions::ReserveAccountName >();
    this->registerTransaction < Volition::Transactions::RestrictAccount >();
    this->registerTransaction < Volition::Transactions::RestrictKey >();
    this->registerTransaction < Volition::Transactions::RunScript >();
    this->registerTransaction < Volition::Transactions::SendAssets >();
    this->registerTransaction < Volition::Transactions::SendVOL >();
    this->registerTransaction < Volition::Transactions::SetEntitlements >();
    this->registerTransaction < Volition::Transactions::UpgradeAssets >();    
}

//----------------------------------------------------------------//
TheTransactionBodyFactory::~TheTransactionBodyFactory () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
