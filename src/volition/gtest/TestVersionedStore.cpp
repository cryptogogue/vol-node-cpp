// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/VersionedStore.h>

using namespace Volition;

static const string KEY     = "test";
static const string STR0    = "abc";
static const string STR1    = "def";
static const string STR2    = "ghi";
static const string STR3    = "jkl";

//----------------------------------------------------------------//
TEST ( VersionedStore, test0 ) {

    VersionedStore store0;
    VersionedStore store1;
    VersionedStore store2;
 
    store0.setDebugName ( "store0" );
    store1.setDebugName ( "store1" );
    store2.setDebugName ( "store2" );
 
    store0.setValue < string >( KEY, STR0 );
    store0.pushVersion ();
    store0.setValue < string >( KEY, STR1 );
    
    store1.takeSnapshot ( store0 );
    store0.setValue < string >( KEY, STR2 );
    
    store2.takeSnapshot ( store0 );
    store0.setValue < string >( KEY, STR3 );
    
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == STR3 );
    ASSERT_TRUE ( store1.getValue < string >( KEY ) == STR1 );
    ASSERT_TRUE ( store2.getValue < string >( KEY ) == STR2 );
    
    ASSERT_TRUE ( store0.getVersion () == 1 );
    ASSERT_TRUE ( store1.getVersion () == 1 );
    ASSERT_TRUE ( store2.getVersion () == 1 );
}

//----------------------------------------------------------------//
TEST ( VersionedStore, test1 ) {

    VersionedStore store0;
    VersionedStore store1;
 
    store0.setValue < string >( KEY, STR0 );
    store1.takeSnapshot ( store0 );
    store0.setValue < string >( KEY, STR1 );
    
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == STR1 );
    ASSERT_TRUE ( store0.getVersion () == 0 );
    
    ASSERT_TRUE ( store1.getValue < string >( KEY ) == STR0 );
    ASSERT_TRUE ( store1.getVersion () == 0 );
}

//----------------------------------------------------------------//
TEST ( VersionedStore, test2 ) {

    VersionedStore store0;
    
    store0.popVersion ();
    
    ASSERT_TRUE ( store0.getValueOrNil < string >( KEY ) == NULL );
    ASSERT_TRUE ( store0.getVersion () == 0 );
    
    store0.setValue < string >( KEY, STR0 );
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == STR0 );
    ASSERT_TRUE ( store0.getVersion () == 0 );
    
    store0.pushVersion ();
    store0.setValue < string >( KEY, STR1 );
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == STR1 );
    ASSERT_TRUE ( store0.getVersion () == 1 );
    
    store0.popVersion ();
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == STR0 );
    ASSERT_TRUE ( store0.getVersion () == 0 );
    
    store0.popVersion ();
    ASSERT_TRUE ( store0.getValueOrNil < string >( KEY ) == NULL );
    ASSERT_TRUE ( store0.getVersion () == 0 );
    
    store0.setValue < string >( KEY, STR0 );
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == STR0 );
    ASSERT_TRUE ( store0.getVersion () == 0 );
}

//----------------------------------------------------------------//
TEST ( VersionedStore, test3 ) {

    VersionedStore store0;
    VersionedStore store1;
    VersionedStore store2;
    
    // set a v0 value
    store0.setValue < string >( KEY, STR0 );
    
    // sanity test - get back the value we set
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == STR0 );
    
    // new version
    store0.pushVersion ();
    
    // should not change value
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == STR0 );
    
    // set a v1 value
    store0.setValue < string >( KEY, STR1 );
    
    // make sure we get our new value
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == STR1 );
    
    // back to v0
    store0.popVersion ();
    
    // should now get the original v0 value
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == STR0 );
    
    // make store1 a snapshot of store0
    store1 = store0;
    
    // make sure we can get our v0 from the store1
    ASSERT_TRUE ( store1.getValue < string >( KEY ) == STR0 );
    
    // now push a new version on the stores
    store0.pushVersion ();
    store1.pushVersion ();
    
    // values in either version should still be v0
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == STR0 );
    ASSERT_TRUE ( store1.getValue < string >( KEY ) == STR0 );
    
    // aset a new value into store0
    store0.setValue < string >( KEY, STR2 );
    
    // the value in store0 should change, but not in store1
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == STR2 );
    ASSERT_TRUE ( store1.getValue < string >( KEY ) == STR0 );
    
    // set a new value into store1
    store1.setValue < string >( KEY, STR3 );
    
    // the value in store0 should change, but not in store1
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == STR2 );
    ASSERT_TRUE ( store1.getValue < string >( KEY ) == STR3 );
    
    // now pop store0...
    store0.popVersion ();
    
    // store0 should be back to the original value
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == STR0 );
    ASSERT_TRUE ( store1.getValue < string >( KEY ) == STR3 );
    
    // and pop store1...
    store1.popVersion ();
    
    // WHAT STRANGE ENCHANTMENT BE THIS?!
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == STR0 );
    ASSERT_TRUE ( store1.getValue < string >( KEY ) == STR0 );
}

//----------------------------------------------------------------//
TEST ( VersionedStore, test4 ) {

    VersionedStore store0;
    VersionedStore store1;

    store0.setValue < string >( KEY, "a" );
    store0.pushVersion ();
    
    store0.setValue < string >( KEY, "b" );
    store0.pushVersion ();
    
    store0.setValue < string >( KEY, "c" );
    store0.pushVersion ();

    store1 = store0;
    
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == "c" );
    ASSERT_TRUE ( store1.getValue < string >( KEY ) == "c" );
    
    store0.setValue < string >( KEY, "aa" );
    store1.setValue < string >( KEY, "bb" );
    
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == "aa" );
    ASSERT_TRUE ( store1.getValue < string >( KEY ) == "bb" );
    
    store0.popVersion ();
    
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == "c" );
    ASSERT_TRUE ( store1.getValue < string >( KEY ) == "bb" );
    
    store1.popVersion ();
    ASSERT_TRUE ( store0.getValue < string >( KEY ) == "c" );
    ASSERT_TRUE ( store1.getValue < string >( KEY ) == "c" );
}
