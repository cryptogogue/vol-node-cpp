// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/VersionedStore.h>
#include <volition/VersionedStoreIterator.h>
#include <volition/VersionedValueIterator.h>

using namespace Volition;

//----------------------------------------------------------------//
void testVersionedStoreIterator ( VersionedStore& store, string key, const vector < string > expected ) {

    VersionedStoreIterator storeIt ( store );

    size_t idx = expected.size ();

    for ( ; storeIt; storeIt.prev ()) {
        ASSERT_TRUE ( storeIt.getValue < string >( key ) == expected [ --idx ]);
    }
    
    ASSERT_TRUE ( idx == 0 );
    
    for ( storeIt.next (); storeIt; storeIt.next ()) {
        ASSERT_TRUE ( storeIt.getValue < string >( key ) == expected [ idx++ ]);
    }
    
    ASSERT_TRUE ( idx == expected.size ());
}

//----------------------------------------------------------------//
void testVersionedValueIterator ( VersionedStore& store, string key, const vector < string > expected ) {

    VersionedValueIterator < string > valueIt ( store, key );

    size_t idx = expected.size ();

    for ( ; valueIt; valueIt.prev ()) {
        ASSERT_TRUE ( *valueIt == expected [ --idx ]);
        ASSERT_TRUE ( valueIt.getValue < string >( key ) == expected [ idx ]);
    }
    
    ASSERT_TRUE ( idx == 0 );
    
    for ( valueIt.next (); valueIt; valueIt.next ()) {
        ASSERT_TRUE ( *valueIt == expected [ idx ]);
        ASSERT_TRUE ( valueIt.getValue < string >( key ) == expected [ idx++ ]);
    }
    
    ASSERT_TRUE ( idx == expected.size ());
}

//----------------------------------------------------------------//
TEST ( VersionedStoreIterators, tes0 ) {

    const string KEY = "test";

    VersionedStore store0;
    VersionedStore store1;

    store0.setValue < string >( KEY, "a" );
    store0.pushVersion ();
    
    store0.setValue < string >( KEY, "b" );
    store0.pushVersion ();
    
    store0.setValue < string >( KEY, "c" );
    store0.pushVersion ();

    store1 = store0;
    
    store0.setValue < string >( KEY, "aa" );
    store0.pushVersion ();

    store0.setValue < string >( KEY, "ab" );
    store0.pushVersion ();

    store0.setValue < string >( KEY, "ac" );

    store1.setValue < string >( KEY, "ba" );
    store1.pushVersion ();

    store1.setValue < string >( KEY, "bb" );
    store1.pushVersion ();

    store1.setValue < string >( KEY, "bc" );
    
    vector < string > expected0 = {
        "a",
        "b",
        "c",
        "aa",
        "ab",
        "ac",
    };
    
    vector < string > expected1 = {
        "a",
        "b",
        "c",
        "ba",
        "bb",
        "bc",
    };
    
    testVersionedStoreIterator ( store0, KEY, expected0 );
    testVersionedStoreIterator ( store1, KEY, expected1 );
    
    testVersionedValueIterator ( store0, KEY, expected0 );
    testVersionedValueIterator ( store1, KEY, expected1 );
}
