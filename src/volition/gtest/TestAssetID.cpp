// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/AssetID.h>

using namespace Volition;

//----------------------------------------------------------------//
TEST ( AssetID, encode_decode ) {

    set < string > collisions;

    for ( u64 i = 0; i < 0xffff; ++i ) {
    
        string assetID = AssetID::encode ( i );
        printf ( "%d: %s\n", ( int )i, assetID.c_str ());
        
        bool isValid;
        u64 index = AssetID::decode ( assetID, &isValid );
        
        ASSERT_TRUE ( index == i );
        ASSERT_TRUE ( isValid );
        ASSERT_TRUE ( collisions.find ( assetID ) == collisions.end ());
        
        collisions.insert ( assetID );
    }
}
