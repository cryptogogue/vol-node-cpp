// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#define JSON_STR(...) #__VA_ARGS__

//static const char* test_block_json = JSON_STR (
//    {
//        "height" : 0,
//        "time" : "1970-01-01T00:00:00-0000",
//        "transactions" : []
//    }
//);

//static const char* test_block_json = JSON_STR (
//    {
//        "height" : 0,
//        "time" : "1970-01-01T00:00:00UTC",
//        "transactions" : [
//            {
//                "bodyIn" : {
//                    "type" :            "GENESIS",
//                    "identity" :        "VOLITION_TEST_NET",
//                    "accounts" : [
//                        {
//                            "name" :            "root",
//                            "grant" :           1000000,
//                            "url" :             "http://127.0.0.1:9090/",
//                            "key" : {
//                                "publicKey" : "-----BEGIN PUBLIC KEY-----\nMFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAEbDmeorItBB1rjT3MpoDHq\/ZR0\/SEyGDF\nIbw0DY8utz2bY++duQlgJsB\/lJNEidf8rH+us0MP\/CEqcGQxTHJvzg==\n-----END PUBLIC KEY-----\n",
//                                "type" : "EC_PEM"
//                            }
//                        },
//                        {
//                            "name" :            "beta",
//                            "grant" :           1000000,
//                            "key" : {
//                                "publicKey" : "-----BEGIN PUBLIC KEY-----\nMFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAES93M5rogLEBC6EfMTiKmiyUEQNU\/GYRL\nmnRQDI9jPnGKzTNjk3IHmvo9KKofd2\/5v+jVFWbRBPqhqaPG5qU3aA==\n-----END PUBLIC KEY-----\n",
//                                "type" : "EC_PEM"
//                            }
//                        },
//                        {
//                            "name" :            "dump",
//                            "grant" :           0,
//                            "key" : {
//                                "publicKey" : "-----BEGIN PUBLIC KEY-----\nMFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAEXa6yyvbFz0fasg47zvlj8r\/GqdDDEbkQ\nMuRhYDxMtYR6ZGDh9svJ045e3YdgmnNC9qprXh0vcwzCEDydKEIKPw==\n-----END PUBLIC KEY-----\n",
//                                "type" : "EC_PEM"
//                            }
//                        }
//                    ]
//                }
//            }
//        ]
//    }
//);
