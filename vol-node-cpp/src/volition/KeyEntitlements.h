// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_KEY_ENTITLEMENTS_H
#define VOLITION_KEY_ENTITLEMENTS_H

#include <volition/common.h>
#include <volition/Entitlements.h>
#include <volition/FNV1a.h>
#include <volition/Singleton.h>

namespace Volition {

//================================================================//
// KeyEntitlements
//================================================================//
class KeyEntitlements {
public:

    static constexpr const char* AFFIRM_KEY         = "AFFIRM_KEY";
    static constexpr const char* BETA_GET_ASSETS    = "BETA_GET_ASSETS";
    static constexpr const char* BETA_GET_DECK      = "BETA_GET_DECK";
    static constexpr const char* OPEN_ACCOUNT       = "OPEN_ACCOUNT";
    static constexpr const char* PUBLISH_SCHEMA     = "PUBLISH_SCHEMA";
    static constexpr const char* REGISTER_MINER     = "REGISTER_MINER";
    static constexpr const char* RENAME_ACCOUNT     = "RENAME_ACCOUNT";
    static constexpr const char* RESTRICT_ACCOUNT   = "RESTRICT_ACCOUNT";
    static constexpr const char* RESTRICT_KEY       = "RESTRICT_KEY";
    static constexpr const char* RUN_SCRIPT         = "RUN_SCRIPT";
    static constexpr const char* SEND_ASSETS        = "SEND_ASSETS";
    static constexpr const char* SEND_VOL           = "SEND_VOL";
    static constexpr const char* SET_ENTITLEMENTS   = "SET_ENTITLEMENTS";
    static constexpr const char* UPGRADE_ASSETS     = "UPGRADE_ASSETS";

    //----------------------------------------------------------------//
    static shared_ptr < Entitlements > getMasterEntitlements () {
        
        shared_ptr < Entitlements > entitlements = make_shared < Entitlements >();
        assert ( entitlements );
        
        entitlements->setPath ( AFFIRM_KEY,             BooleanEntitlement ( true ));
        entitlements->setPath ( BETA_GET_ASSETS,        BooleanEntitlement ( true ));
        entitlements->setPath ( BETA_GET_DECK,          BooleanEntitlement ( true ));
        entitlements->setPath ( OPEN_ACCOUNT,           BooleanEntitlement ( true ));
        entitlements->setPath ( PUBLISH_SCHEMA,         BooleanEntitlement ( true ));
        entitlements->setPath ( REGISTER_MINER,         BooleanEntitlement ( true ));
        entitlements->setPath ( RENAME_ACCOUNT,         BooleanEntitlement ( true ));
        entitlements->setPath ( RESTRICT_ACCOUNT,       BooleanEntitlement ( true ));
        entitlements->setPath ( RESTRICT_KEY,           BooleanEntitlement ( true ));
        entitlements->setPath ( RUN_SCRIPT,             BooleanEntitlement ( true ));
        entitlements->setPath ( SEND_ASSETS,            BooleanEntitlement ( true ));
        entitlements->setPath ( SEND_VOL,               BooleanEntitlement ( true ));
        entitlements->setPath ( SET_ENTITLEMENTS,       BooleanEntitlement ( true ));
        entitlements->setPath ( UPGRADE_ASSETS,         BooleanEntitlement ( true ));
        return entitlements;
    }
};

} // namespace Volition
#endif
