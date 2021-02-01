// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/Entitlements.h>
#include <volition/serialization/Serialization.h>

using namespace Volition;

#define JSON_STR(...) #__VA_ARGS__

static const char* entitlements0_json = JSON_STR ({

    "type": "path",
    "children": {
        "p0": {
            "type": "path",
            "children": {
                "p0": {
                    "type": "boolean",
                    "value": true
                },
                "p1": {
                    "type": "boolean",
                    "value": false
                }
            }
        },
        "p1": {
            "type": "numeric",
            "limit": 0
        },
        "p2": {
            "type": "numeric",
            "lower": {
                "enabled": true,
                "exclude": false,
                "limit": 10
            }
        },
        "p3": {
            "type": "numeric",
            "upper": {
                "enabled": true,
                "exclude": false,
                "limit": 100
            }
        }
    }
});

static const char* entitlements0_subset_json = JSON_STR ({

    "type": "path",
    "children": {
        "p0": {
            "type": "path",
            "children": {
                "p0": {
                    "type": "boolean",
                    "value": false
                }
            }
        },
        "p2": {
            "type": "numeric",
            "lower": {
                "enabled": true,
                "exclude": false,
                "limit": 25
            }
        },
        "p3": {
            "type": "numeric",
            "upper": {
                "enabled": true,
                "exclude": false,
                "limit": 75
            }
        }
    }
});

//----------------------------------------------------------------//
TEST ( Entitlements, entitlements ) {
    
    Entitlements entitlements0;
    FromJSONSerializer::fromJSONString ( entitlements0, entitlements0_json );
    
    ASSERT_TRUE     ( entitlements0.check ( "" )); // exists
    ASSERT_TRUE     ( entitlements0.check ( "p0" )); // exists
    ASSERT_TRUE     ( entitlements0.check ( "p0.p0" )); // leaf value is true
    ASSERT_FALSE    ( entitlements0.check ( "p0.p1" )); // leaf value is false
    ASSERT_FALSE    ( entitlements0.check ( "p0.p2" )); // doesn't exist
    
    ASSERT_TRUE     ( entitlements0.check ( "p1" )); // numeric with no limits; always true
    ASSERT_TRUE     ( entitlements0.check ( "p1", 10000 )); // always true
    
    ASSERT_FALSE    ( entitlements0.check ( "p2" )); // numeric with no limits; false if bool
    ASSERT_TRUE     ( entitlements0.check ( "p2", 100 )); // lower bound is 10
    ASSERT_FALSE    ( entitlements0.check ( "p2", 5 )); // lower bound is 10
    
    ASSERT_FALSE    ( entitlements0.check ( "p3" )); // numeric with no limits; false if bool
    ASSERT_TRUE     ( entitlements0.check ( "p3", 50 )); // upper bound is 100
    ASSERT_FALSE    ( entitlements0.check ( "p3", 150 )); // lower bound is 100
    
    ASSERT_TRUE ( entitlements0.isMatchOrSubsetOf ( &entitlements0 )); // policy should match self
    
    Entitlements entitlements0_subset;
    FromJSONSerializer::fromJSONString ( entitlements0_subset, entitlements0_subset_json );
    
    ASSERT_TRUE ( entitlements0_subset.isMatchOrSubsetOf ( &entitlements0 ));
    ASSERT_FALSE ( entitlements0.isMatchOrSubsetOf ( &entitlements0_subset ));
}

//----------------------------------------------------------------//
TEST ( Entitlements, entitlements_affirm_path ) {
    
    Entitlements entitlements;
    
    entitlements.setPath ( "p0" );
    ASSERT_TRUE ( entitlements.check ( "p0" ));
    ASSERT_FALSE ( entitlements.check ( "p1" ));
    ASSERT_FALSE ( entitlements.check ( "p0.p1" ));
    
    entitlements.setPath ( "p0.p1" );
    ASSERT_TRUE ( entitlements.check ( "p0" ));
    ASSERT_TRUE ( entitlements.check ( "p0.p1" ));
    
    entitlements.setPath ( "p0.p1.p2" );
    ASSERT_TRUE ( entitlements.check ( "p0" ));
    ASSERT_TRUE ( entitlements.check ( "p0.p1" ));
    ASSERT_TRUE ( entitlements.check ( "p0.p1.p2" ));
    
    entitlements.setPath ( "p0.p1.p2", BooleanEntitlement ( true ));
    ASSERT_TRUE ( entitlements.check ( "p0.p1.p2" ));
    
    entitlements.setPath ( "p0.p1.p2.p3",  BooleanEntitlement ( true ));
    ASSERT_TRUE ( entitlements.check ( "p0.p1.p2.p3" ));
}

//----------------------------------------------------------------//
TEST ( Entitlements, entitlements_apply ) {
    
    Entitlements e0;
    e0.setPath ( "p0.p0.p0",    BooleanEntitlement ( true ));
    e0.setPath ( "p0.p0.p1",    BooleanEntitlement ( false ));
    
    ASSERT_TRUE ( e0.check ( "p0.p0.p0" ));
    ASSERT_FALSE ( e0.check ( "p0.p0.p1" ));
    
    Entitlements e1;
    e1.setPath ( "p0.p0.p1",    BooleanEntitlement ( true ));
    e1.setPath ( "p0.p1",       BooleanEntitlement ( true ));
    
    ASSERT_TRUE ( e1.check ( "p0.p0.p1" ));
    ASSERT_TRUE ( e1.check ( "p0.p1" ));
    
    Entitlements e2 ( *e1.apply ( e0 ));
    
    ASSERT_TRUE ( e2.check ( "p0.p0.p0" ));
    ASSERT_TRUE ( e2.check ( "p0.p0.p1" ));
    ASSERT_TRUE ( e2.check ( "p0.p1" ));
}

//----------------------------------------------------------------//
TEST ( Entitlements, entitlements_prune ) {
    
    Entitlements e0;
    e0.setPath ( "p0.p0.p0",    BooleanEntitlement ( true ));
    e0.setPath ( "p0.p0.p1",    BooleanEntitlement ( false ));
    
    Entitlements e1;
    e1.setPath ( "p0.p0.p1",    BooleanEntitlement ( false ));
    
    ASSERT_TRUE ( e1.check ( "p0" ));
    
    Entitlements e2 ( *e1.prune ( e0 ));
    
    ASSERT_FALSE ( e2.check ( "p0" ));
}
