// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/ControlCommands.h>
#include <volition/TheControlCommandBodyFactory.h>

namespace Volition {

//================================================================//
// TheControlCommandBodyFactory
//================================================================//

//----------------------------------------------------------------//
unique_ptr < AbstractControlCommandBody > TheControlCommandBodyFactory::create ( string typeString ) const {

    return this->Factory::create ( typeString );
}

//----------------------------------------------------------------//
TheControlCommandBodyFactory::TheControlCommandBodyFactory () {

    this->registerTransaction < Volition::Commands::HardReset >();
    this->registerTransaction < Volition::Commands::SelectReward >();
    this->registerTransaction < Volition::Commands::SetMinimumGratuity >();
}

//----------------------------------------------------------------//
TheControlCommandBodyFactory::~TheControlCommandBodyFactory () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
