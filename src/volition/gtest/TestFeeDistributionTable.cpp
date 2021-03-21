// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/PayoutPolicy.h>

using namespace Volition;

//----------------------------------------------------------------//
TEST ( PayoutPolicy, simple_distribution ) {
    
    {
        PayoutPolicy table;
        table.setScale ( 10 );
        table.setShare ( "foo", 5 );
        table.setShare ( "bar", 5 );
        ASSERT_TRUE ( table.isBalanced ());
    }
    
    {
        PayoutPolicy table;
        table.setScale ( 100 );
        table.setShare ( "foo", 50 );
        table.setShare ( "bar", 25 );
        table.setShare ( "baz", 25 );
        ASSERT_TRUE ( table.isBalanced ());
    }

    {
        PayoutPolicy table;
        table.setScale ( 1000 );
        table.setShare ( "foo", 666 );
        table.setShare ( "bar", 333 );
        table.setShare ( "baz", 1 );
        ASSERT_TRUE ( table.isBalanced ());
    }
}

//----------------------------------------------------------------//
TEST ( FeePercent, fixedPoint ) {
    
    {
        FeePercent percent ( 10, 5 );
        ASSERT_TRUE ( percent.computeAndRoundDown ( 1000 ) == 500 );
        ASSERT_TRUE ( percent.computeAndRoundDown ( 1001 ) == 500 );
        ASSERT_TRUE ( percent.computeAndRoundUp ( 1001 ) == 501 );
    }
    
    {
        FeePercent percent ( 2, 1 );
        ASSERT_TRUE ( percent.computeAndRoundDown ( 1000 ) == 500 );
        ASSERT_TRUE ( percent.computeAndRoundDown ( 1001 ) == 500 );
        ASSERT_TRUE ( percent.computeAndRoundUp ( 1001 ) == 501 );
    }
    
    {
        FeePercent percent ( 9, 3 );
        ASSERT_TRUE ( percent.computeAndRoundDown ( 100 ) == 33 );
        ASSERT_TRUE ( percent.computeAndRoundUp ( 100 ) == 34 );
    }
}
