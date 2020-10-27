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
    
    Schema updateSchema;
    FromJSONSerializer::fromJSONString ( updateSchema, schema_json );
    
    Schema schema;
    ledger.getSchema ( schema );
    schema.compose ( updateSchema );
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
    
    map < string, size_t > histogram;
    
    SerializableList < SerializableSharedConstPtr < Asset >> inventory;
    ledger.getInventory ( schema, accountID, inventory );
    
    SerializableList < SerializableSharedConstPtr < Asset >>::const_iterator inventoryIt = inventory.cbegin ();
    for ( ; inventoryIt != inventory.cend (); ++inventoryIt ) {
        SerializableSharedConstPtr < Asset > asset = *inventoryIt;
    
        if ( histogram.find ( asset->mType ) == histogram.cend ()) {
            histogram [ asset->mType ] = 0;
        }
        histogram [ asset->mType ] = histogram [ asset->mType ] + 1;
    }
    
    ASSERT_TRUE ( histogram [ "common" ] == 3 );
    ASSERT_TRUE ( histogram [ "rare" ] == 2 );
    ASSERT_TRUE ( histogram [ "ultrarare" ] == 1 );
    ASSERT_TRUE ( histogram.find ( "pack" ) == histogram.cend ());
}
