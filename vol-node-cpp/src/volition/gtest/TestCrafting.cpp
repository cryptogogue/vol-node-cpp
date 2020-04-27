// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/AssetMethodInvocation.h>
#include <volition/Ledger.h>
#include <volition/Schema.h>
#include <volition/serialization/Serialization.h>

#include "TestCrafting.json.h"

using namespace Volition;

//----------------------------------------------------------------//
TEST ( Crafting, crafting ) {
    
    Ledger ledger;
    ledger.init ();
    
    Schema updateSchema;
    FromJSONSerializer::fromJSONString ( updateSchema, schema_json );
    
    Schema schema;
    ledger.getSchema ( schema );
    schema.compose ( updateSchema );
    ledger.setObject < Schema >( FormatLedgerKey::forSchema (), schema );
    
    CryptoKey key;
    key.elliptic ();
    
    Policy keyPolicy;
    ledger.getEntitlements < KeyEntitlements >( keyPolicy );
    
    Policy accountPolicy;
    ledger.getEntitlements < AccountEntitlements >( accountPolicy );
    
    ledger.newAccount ( "test", 1000, "master", key, keyPolicy, accountPolicy );
    
    ledger.awardAsset ( schema, "test", "pack", 1 );
    
    AssetMethodInvocation invocation;
    invocation.setMethod ( "openPack" );
    invocation.setAssetParam ( "pack", 0 );
    ledger.invoke ( schema, "test", invocation );
    
    map < string, size_t > histogram;
    
    SerializableList < Asset > inventory = ledger.getInventory ( schema, "test" );
    SerializableList < Asset >::const_iterator inventoryIt = inventory.cbegin ();
    for ( ; inventoryIt != inventory.cend (); ++inventoryIt ) {
        const Asset& asset = *inventoryIt;
    
        if ( histogram.find ( asset.mType ) == histogram.cend ()) {
            histogram [ asset.mType ] = 0;
        }
        histogram [ asset.mType ] = histogram [ asset.mType ] + 1;
    }
    
    ASSERT_TRUE ( histogram [ "common" ] == 3 );
    ASSERT_TRUE ( histogram [ "rare" ] == 2 );
    ASSERT_TRUE ( histogram [ "ultrarare" ] == 1 );
    ASSERT_TRUE ( histogram.find ( "pack" ) == histogram.cend ());
}
