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

     //----------------------------------------------------------------//
    static bool canOpenAccount ( const Entitlements& entitlements ) {
    
        return entitlements.check ( "OPEN_ACCOUNT" );
    }

    //----------------------------------------------------------------//
    static bool canPerformTransaction ( const Entitlements& entitlements, string transactionType ) {
    
        return entitlements.check ( transactionType );
    }

    //----------------------------------------------------------------//
    static shared_ptr < Entitlements > getMasterEntitlements () {
        
        shared_ptr < Entitlements > entitlements = make_shared < Entitlements >();
        assert ( entitlements );
        
        entitlements->setPath ( "AFFIRM_KEY",           BooleanEntitlement ( true ));
        entitlements->setPath ( "BETA_GET_ASSETS",      BooleanEntitlement ( true ));
        entitlements->setPath ( "OPEN_ACCOUNT",         BooleanEntitlement ( true ));
        entitlements->setPath ( "PUBLISH_SCHEMA",       BooleanEntitlement ( true ));
        entitlements->setPath ( "REGISTER_MINER",       BooleanEntitlement ( true ));
        entitlements->setPath ( "RENAME_ACCOUNT",       BooleanEntitlement ( true ));
        entitlements->setPath ( "SEND_ASSETS",          BooleanEntitlement ( true ));
        entitlements->setPath ( "SEND_VOL",             BooleanEntitlement ( true ));
        entitlements->setPath ( "UPGRADE_ASSETS",       BooleanEntitlement ( true ));
        return entitlements;
    }
};

} // namespace Volition
#endif
