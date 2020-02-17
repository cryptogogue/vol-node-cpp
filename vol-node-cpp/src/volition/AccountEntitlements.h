// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ACCOUNT_ENTITLEMENTS_H
#define VOLITION_ACCOUNT_ENTITLEMENTS_H

#include <volition/common.h>
#include <volition/Entitlements.h>

namespace Volition {

//================================================================//
// AccountEntitlements
//================================================================//
class AccountEntitlements {
public:

    static constexpr const char* MAX_ASSETS         = "MAX_ASSETS";
    static constexpr const char* MAX_VOL            = "MAX_VOL";

    //----------------------------------------------------------------//
    static shared_ptr < Entitlements > getMasterEntitlements () {
    
        shared_ptr < Entitlements > entitlements = make_shared < Entitlements >();
        assert ( entitlements );
        
        entitlements->setPath ( MAX_ASSETS,         NumericEntitlement ());
        entitlements->setPath ( MAX_VOL,            NumericEntitlement ());
        return entitlements;
    }
};

} // namespace Volition
#endif
