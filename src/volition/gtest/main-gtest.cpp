// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>

//----------------------------------------------------------------//
int main ( int argc, char **argv ) {

    ::testing::InitGoogleTest ( &argc, argv );
    int result = RUN_ALL_TESTS ();
    return result;
}
