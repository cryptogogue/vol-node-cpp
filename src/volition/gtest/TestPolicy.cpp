// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/EntitlementFactory.h>
#include <volition/Policy.h>
#include <volition/serialization/Serialization.h>

using namespace Volition;

#define JSON_STR(...) #__VA_ARGS__

static const char* policy0_json = JSON_STR ({

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

static const char* policy0_subset_json = JSON_STR ({

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
TEST ( Policy, entitlements ) {
    
    SerializableSharedPtr < AbstractEntitlement, EntitlementFactory > policy0;
    FromJSONSerializer::fromJSONString ( policy0, policy0_json );
    
    ASSERT_TRUE     ( policy0 != NULL );
    ASSERT_TRUE     ( policy0->check ( "" )); // exists
    ASSERT_TRUE     ( policy0->check ( "p0" )); // exists
    ASSERT_TRUE     ( policy0->check ( "p0.p0" )); // leaf value is true
    ASSERT_FALSE    ( policy0->check ( "p0.p1" )); // leaf value is false
    ASSERT_FALSE    ( policy0->check ( "p0.p2" )); // doesn't exist
    
    ASSERT_FALSE    ( policy0->check ( "p1" )); // not a bool
    ASSERT_TRUE     ( policy0->check ( "p1", 10000 )); // any value permitted
    
    ASSERT_FALSE    ( policy0->check ( "p2" )); // not a bool
    ASSERT_TRUE     ( policy0->check ( "p2", 100 )); // lower bound is 10
    ASSERT_FALSE    ( policy0->check ( "p2", 5 )); // lower bound is 10
    
    ASSERT_FALSE    ( policy0->check ( "p3" )); // not a bool
    ASSERT_TRUE     ( policy0->check ( "p3", 50 )); // upper bound is 100
    ASSERT_FALSE    ( policy0->check ( "p3", 150 )); // lower bound is 100
    
    ASSERT_TRUE ( policy0->isMatchOrSubsetOf ( policy0.get ())); // policy should match self
    
    SerializableSharedPtr < AbstractEntitlement, EntitlementFactory > policy0_subset;
    FromJSONSerializer::fromJSONString ( policy0_subset, policy0_subset_json );
    
    ASSERT_TRUE ( policy0_subset->isMatchOrSubsetOf ( policy0.get ()));
    ASSERT_FALSE ( policy0->isMatchOrSubsetOf ( policy0_subset.get ()));
}
