// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_THEMASTERENTITLEMENTS_H
#define VOLITION_THEMASTERENTITLEMENTS_H

#include <volition/common.h>
#include <volition/Entitlements.h>
#include <volition/Singleton.h>

namespace Volition {

//================================================================//
// TheMasterAccountEntitlements
//================================================================//
class TheMasterAccountEntitlements :
    public Singleton < TheMasterAccountEntitlements >,
    public Entitlements {
public:

    //----------------------------------------------------------------//
    TheMasterAccountEntitlements () {
    
        this->affirmPath ( "maxAssets",          make_shared < NumericEntitlement >());
        this->affirmPath ( "maxVOL",             make_shared < NumericEntitlement >());
    }
};

//================================================================//
// TheMasterKeyEntitlements
//================================================================//
class TheMasterKeyEntitlements :
    public Singleton < TheMasterKeyEntitlements >,
    public Entitlements {
public:

    //----------------------------------------------------------------//
    TheMasterKeyEntitlements () {
    
        this->affirmPath ( "affirmKey",          make_shared < BooleanEntitlement >( true ));
        this->affirmPath ( "betaGetAssets",      make_shared < BooleanEntitlement >( true ));
        this->affirmPath ( "openAccount",        make_shared < BooleanEntitlement >( true ));
        this->affirmPath ( "publishSchema",      make_shared < BooleanEntitlement >( true ));
        this->affirmPath ( "registerMiner",      make_shared < BooleanEntitlement >( true ));
        this->affirmPath ( "renameAccount",      make_shared < BooleanEntitlement >( true ));
        this->affirmPath ( "sendAssets",         make_shared < BooleanEntitlement >( true ));
        this->affirmPath ( "sendVOL",            make_shared < BooleanEntitlement >( true ));
    }
};

} // namespace Volition
#endif
