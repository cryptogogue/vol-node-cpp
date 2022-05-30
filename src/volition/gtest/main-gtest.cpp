// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/common.h>

//----------------------------------------------------------------//
int main ( int argc, char **argv ) {

    Lognosis::setFilter ( PDM_FILTER_ROOT, Lognosis::OFF );
    Lognosis::init ( argc, argv );

    if ( sodium_init () < 0 ) {
        assert ( false );
    }

    ::testing::InitGoogleTest ( &argc, argv );
    int result = RUN_ALL_TESTS ();
    return result;
}
