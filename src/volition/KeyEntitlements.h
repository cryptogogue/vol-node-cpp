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

    // TODO: these constants should be in the transaction class, not here
    static constexpr const char* AFFIRM_KEY                     = "AFFIRM_KEY";
    static constexpr const char* BETA_GET_ASSETS                = "BETA_GET_ASSETS";
    static constexpr const char* BETA_GET_DECK                  = "BETA_GET_DECK";
    static constexpr const char* BUY_ASSETS                     = "BUY_ASSETS";
    static constexpr const char* HARD_RESET                     = "HARD_RESET";
    static constexpr const char* OFFER_ASSETS                   = "OFFER_ASSETS";
    static constexpr const char* OPEN_ACCOUNT                   = "OPEN_ACCOUNT";
    static constexpr const char* PUBLISH_SCHEMA                 = "PUBLISH_SCHEMA";
    static constexpr const char* PUBLISH_SCHEMA_AND_RESET       = "PUBLISH_SCHEMA_AND_RESET";
    static constexpr const char* REGISTER_MINER                 = "REGISTER_MINER";
    static constexpr const char* RENAME_ACCOUNT                 = "RENAME_ACCOUNT";
    static constexpr const char* RESERVE_ACCOUNT_NAME           = "RESERVE_ACCOUNT_NAME";
    static constexpr const char* RESTRICT_ACCOUNT               = "RESTRICT_ACCOUNT";
    static constexpr const char* RESTRICT_KEY                   = "RESTRICT_KEY";
    static constexpr const char* RUN_SCRIPT                     = "RUN_SCRIPT";
    static constexpr const char* SELECT_REWARD                  = "SELECT_REWARD";
    static constexpr const char* SEND_ASSETS                    = "SEND_ASSETS";
    static constexpr const char* SEND_VOL                       = "SEND_VOL";
    static constexpr const char* SET_ENTITLEMENTS               = "SET_ENTITLEMENTS";
    static constexpr const char* SET_MONETARY_POLICY            = "SET_MONETARY_POLICY";
    static constexpr const char* SET_PAYOUT_POLICY              = "SET_PAYOUT_POLICY";
    static constexpr const char* SET_TERMS_OF_SERVICE           = "SET_TERMS_OF_SERVICE";
    static constexpr const char* SET_TRANSACTION_FEE_SCHEDULE   = "SET_TRANSACTION_FEE_SCHEDULE";
    static constexpr const char* UPDATE_MINER_INFO              = "UPDATE_MINER_INFO";
    static constexpr const char* UPGRADE_ASSETS                 = "UPGRADE_ASSETS";

    //----------------------------------------------------------------//
    static shared_ptr < const Entitlements > getMasterEntitlements () {
        
        shared_ptr < Entitlements > entitlements = make_shared < Entitlements >();
        assert ( entitlements );
        
        entitlements->setPath ( AFFIRM_KEY,                     BooleanEntitlement ( true ));
        entitlements->setPath ( BETA_GET_ASSETS,                BooleanEntitlement ( true ));
        entitlements->setPath ( BETA_GET_DECK,                  BooleanEntitlement ( true ));
        entitlements->setPath ( BUY_ASSETS,                     BooleanEntitlement ( true ));
        entitlements->setPath ( HARD_RESET,                     BooleanEntitlement ( true ));
        entitlements->setPath ( OFFER_ASSETS,                   BooleanEntitlement ( true ));
        entitlements->setPath ( OPEN_ACCOUNT,                   BooleanEntitlement ( true ));
        entitlements->setPath ( PUBLISH_SCHEMA,                 BooleanEntitlement ( true ));
        entitlements->setPath ( PUBLISH_SCHEMA_AND_RESET,       BooleanEntitlement ( true ));
        entitlements->setPath ( REGISTER_MINER,                 BooleanEntitlement ( true ));
        entitlements->setPath ( RENAME_ACCOUNT,                 BooleanEntitlement ( true ));
        entitlements->setPath ( RESERVE_ACCOUNT_NAME,           BooleanEntitlement ( true ));
        entitlements->setPath ( RESTRICT_ACCOUNT,               BooleanEntitlement ( true ));
        entitlements->setPath ( RESTRICT_KEY,                   BooleanEntitlement ( true ));
        entitlements->setPath ( RUN_SCRIPT,                     BooleanEntitlement ( true ));
        entitlements->setPath ( SELECT_REWARD,                  BooleanEntitlement ( true ));
        entitlements->setPath ( SEND_ASSETS,                    BooleanEntitlement ( true ));
        entitlements->setPath ( SEND_VOL,                       BooleanEntitlement ( true ));
        entitlements->setPath ( SET_ENTITLEMENTS,               BooleanEntitlement ( true ));
        entitlements->setPath ( SET_MONETARY_POLICY,            BooleanEntitlement ( true ));
        entitlements->setPath ( SET_PAYOUT_POLICY,              BooleanEntitlement ( true ));
        entitlements->setPath ( SET_TERMS_OF_SERVICE,           BooleanEntitlement ( true ));
        entitlements->setPath ( SET_TRANSACTION_FEE_SCHEDULE,   BooleanEntitlement ( true ));
        entitlements->setPath ( UPDATE_MINER_INFO,              BooleanEntitlement ( true ));
        entitlements->setPath ( UPGRADE_ASSETS,                 BooleanEntitlement ( true ));

        return entitlements;
    }
    
    //----------------------------------------------------------------//
    static shared_ptr < const Entitlements > getMiningKeyEntitlements () {
        
        shared_ptr < Entitlements > entitlements = make_shared < Entitlements >();
        assert ( entitlements );
        
        entitlements->setPath ( AFFIRM_KEY,                     BooleanEntitlement ( false ));
        entitlements->setPath ( BETA_GET_ASSETS,                BooleanEntitlement ( false ));
        entitlements->setPath ( BETA_GET_DECK,                  BooleanEntitlement ( false ));
        entitlements->setPath ( BUY_ASSETS,                     BooleanEntitlement ( false ));
        entitlements->setPath ( HARD_RESET,                     BooleanEntitlement ( false ));
        entitlements->setPath ( OFFER_ASSETS,                   BooleanEntitlement ( false ));
        entitlements->setPath ( OPEN_ACCOUNT,                   BooleanEntitlement ( false ));
        entitlements->setPath ( PUBLISH_SCHEMA,                 BooleanEntitlement ( false ));
        entitlements->setPath ( PUBLISH_SCHEMA_AND_RESET,       BooleanEntitlement ( false ));
        entitlements->setPath ( REGISTER_MINER,                 BooleanEntitlement ( false ));
        entitlements->setPath ( RENAME_ACCOUNT,                 BooleanEntitlement ( false ));
        entitlements->setPath ( RESERVE_ACCOUNT_NAME,           BooleanEntitlement ( false ));
        entitlements->setPath ( RESTRICT_ACCOUNT,               BooleanEntitlement ( false ));
        entitlements->setPath ( RESTRICT_KEY,                   BooleanEntitlement ( false ));
        entitlements->setPath ( RUN_SCRIPT,                     BooleanEntitlement ( false ));
        entitlements->setPath ( SELECT_REWARD,                  BooleanEntitlement ( false ));
        entitlements->setPath ( SEND_ASSETS,                    BooleanEntitlement ( true ));
        entitlements->setPath ( SEND_VOL,                       BooleanEntitlement ( true ));
        entitlements->setPath ( SET_ENTITLEMENTS,               BooleanEntitlement ( false ));
        entitlements->setPath ( SET_MONETARY_POLICY,            BooleanEntitlement ( false ));
        entitlements->setPath ( SET_PAYOUT_POLICY,              BooleanEntitlement ( false ));
        entitlements->setPath ( SET_TERMS_OF_SERVICE,           BooleanEntitlement ( false ));
        entitlements->setPath ( SET_TRANSACTION_FEE_SCHEDULE,   BooleanEntitlement ( false ));
        entitlements->setPath ( UPDATE_MINER_INFO,              BooleanEntitlement ( false ));
        entitlements->setPath ( UPGRADE_ASSETS,                 BooleanEntitlement ( false ));

        return entitlements;
    }
};

} // namespace Volition
#endif
