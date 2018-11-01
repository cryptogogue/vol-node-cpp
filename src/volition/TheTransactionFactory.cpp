// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/TheTransactionFactory.h>
#include <volition/transactions/AccountPolicy.h>
#include <volition/transactions/AffirmKey.h>
#include <volition/transactions/GenesisMiner.h>
#include <volition/transactions/KeyPolicy.h>
#include <volition/transactions/OpenAccount.h>
#include <volition/transactions/PublishSchema.h>
#include <volition/transactions/RegisterMiner.h>
#include <volition/transactions/SellAssets.h>
#include <volition/transactions/SendAssets.h>
#include <volition/transactions/SendVOL.h>
#include <volition/transactions/TransformAssets.h>

// ACCOUNT_POLICY (account, policy)
// AFFIRM_KEY (account name, key name, policy name, (opt)key)
// DELETE_KEY (account name, key name)
// GENESIS_MINER ( account name, key name, url, amount)
// KEY_POLICY (account, policy, policy name)
// OPEN_ACCOUNT (account name, master key, key name)
// PUBLISH_SCHEMA (json, lua)
// REGISTER_MINER (account name, url)
// SELL_ASSETS (assets, buyer, seller, amount)
// SEND_ASSETS (assets, from, to)
// SEND_VOL (from, to)
// TRANSFORM_ASSETS (...)

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

    this->registerTransaction < Volition::Transactions::AccountPolicy >();
    this->registerTransaction < Volition::Transactions::AffirmKey >();
    this->registerTransaction < Volition::Transactions::GenesisMiner >();
    this->registerTransaction < Volition::Transactions::KeyPolicy >();
    this->registerTransaction < Volition::Transactions::OpenAccount >();
    this->registerTransaction < Volition::Transactions::PublishSchema >();
    this->registerTransaction < Volition::Transactions::RegisterMiner >();
    this->registerTransaction < Volition::Transactions::SellAssets >();
    this->registerTransaction < Volition::Transactions::SendAssets >();
    this->registerTransaction < Volition::Transactions::SendVOL >();
    this->registerTransaction < Volition::Transactions::TransformAssets >();
}

//----------------------------------------------------------------//
TheTransactionFactory::~TheTransactionFactory () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
