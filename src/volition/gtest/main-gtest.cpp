// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/common.h>

//----------------------------------------------------------------//
int main ( int argc, char **argv ) {

    loguru::init ( argc, argv );
    loguru::g_preamble = false;
    LOG_F ( INFO, "Hello from main.cpp!" );

    ::testing::InitGoogleTest ( &argc, argv );
    int result = RUN_ALL_TESTS ();
    return result;
}
