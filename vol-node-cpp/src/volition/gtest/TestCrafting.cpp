// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/AssetMethodInvocation.h>
#include <volition/CryptoKey.h>
#include <volition/Ledger.h>
#include <volition/Schema.h>
#include <volition/serialization/Serialization.h>

#include "TestCrafting.json.h"

using namespace Volition;

//----------------------------------------------------------------//
TEST ( Crafting, crafting ) {
    
    time_t t;
    time ( &t );
    
    LedgerResult result = false;
    
    Ledger ledger;
    ledger.init ();
    
    Schema schema;
    FromJSONSerializer::fromJSONString ( schema, schema_json );
    ledger.setSchema ( schema );
    
    CryptoKeyPair key;
    key.elliptic ();
    
    Policy keyPolicy;
    ledger.getEntitlements < KeyEntitlements >( keyPolicy );
    
    Policy accountPolicy;
    ledger.getEntitlements < AccountEntitlements >( accountPolicy );
    
    result = ledger.newAccount ( "test", 1000, "master", key.getPublicKey (), keyPolicy, accountPolicy );
    ASSERT_TRUE ( result );
    
    AccountID accountID = ledger.getAccountID ( "test" );
    ASSERT_TRUE ( accountID != AccountID::NULL_INDEX );
    
    result = ledger.awardAssets ( schema, accountID, "pack", 1, t );
    ASSERT_TRUE ( result );
    
    AssetMethodInvocation invocation;
    invocation.setMethod ( "openPack" );
    invocation.setAssetParam ( "pack", 0 );
    result = ledger.invoke ( schema, "test", invocation, t );
    ASSERT_TRUE ( result );
    
    map < string, size_t > histogram = ledger.getInventoryHistogram ( schema, accountID );
    
    ASSERT_TRUE ( histogram [ "common" ] == 3 );
    ASSERT_TRUE ( histogram [ "rare" ] == 2 );
    ASSERT_TRUE ( histogram [ "ultrarare" ] == 1 );
    ASSERT_TRUE ( histogram.find ( "pack" ) == histogram.cend ());
}

//----------------------------------------------------------------//
TEST ( Rewards, rewards ) {
    
    time_t t;
    time ( &t );

    LedgerResult result = false;

    Ledger ledger;
    ledger.init ();

    Schema schema;
    FromJSONSerializer::fromJSONString ( schema, schema_json );
    ledger.setSchema ( schema );

    CryptoKeyPair key;
    key.elliptic ();

    Policy keyPolicy;
    ledger.getEntitlements < KeyEntitlements >( keyPolicy );

    Policy accountPolicy;
    ledger.getEntitlements < AccountEntitlements >( accountPolicy );

    result = ledger.newAccount ( "test", 1000, "master", key.getPublicKey (), keyPolicy, accountPolicy );
    ASSERT_TRUE ( result );

    AccountID accountID = ledger.getAccountID ( "test" );
    ASSERT_TRUE ( accountID != AccountID::NULL_INDEX );

    // boosterBox has a limit of 3 rewards
    for ( size_t i = 0; i < 5; ++i ) {
        result = ledger.invokeReward ( schema, "test", "boosterBox", t );
        ASSERT_TRUE ( result );
    }

    map < string, size_t > histogram = ledger.getInventoryHistogram ( schema, accountID );
    ASSERT_TRUE ( histogram [ "pack" ] == 36 );
}

