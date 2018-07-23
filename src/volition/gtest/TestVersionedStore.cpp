// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/VersionedStore.h>

using namespace Volition;

//----------------------------------------------------------------//
TEST ( VersionedStore, unitTests ) {

    VersionedStore store;
    
    store.setValue < string >( "test", "abc" );
    string result = store.getValue < string >( "test" );
    
    ASSERT_TRUE ( result == "abc" );
}
