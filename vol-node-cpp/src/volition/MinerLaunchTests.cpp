// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractChainRecorder.h>
#include <volition/Block.h>
#include <volition/Digest.h>
#include <volition/Miner.h>
#include <volition/MinerLaunchTests.h>
#include <volition/MinerLaunchTests.json.h>
#include <volition/TheContext.h>
#include <volition/SyncChainTask.h>

namespace Volition {

//================================================================//
// MinerLaunchTests
//================================================================//

//----------------------------------------------------------------//
bool MinerLaunchTests::checkDeterministic ( const CryptoKey& key, size_t cycles, size_t expectedResult ) {

    set < string > sigSet;
    
    for ( size_t i = 0; i < cycles; ++i ) {
        Signature sig = key.sign ( "abc", Digest::HASH_ALGORITHM_SHA256 );
        sigSet.insert ( sig.getSignature ().toHex ());
    }
    return ( sigSet.size () == expectedResult );
}

//----------------------------------------------------------------//
void MinerLaunchTests::checkEnvironment () {

    assert ( sizeof ( s8 ) == 1 );
    assert ( sizeof ( s16 ) == 2 );
    assert ( sizeof ( s32 ) == 4 );
    assert ( sizeof ( s64 ) == 8 );
    
    assert ( sizeof ( u8 ) == 1 );
    assert ( sizeof ( u16 ) == 2 );
    assert ( sizeof ( u32 ) == 4 );
    assert ( sizeof ( u64 ) == 8 );
    
    assert ( sizeof ( time_t ) == 8 );
    assert ( sizeof ( size_t ) == 8 );
            
    SerializableSet < string > hashSet;
    
    hashSet.insert ( "abc" );
    assert ( ToJSONSerializer::toDigestString ( hashSet ) == JSON_STR (["abc"]));
    assert ( Digest ( hashSet, Digest::HASH_ALGORITHM_SHA256 ).toHex () == "02f393ea9358560882c1fe797bf99d600aa4643a68276d8e3d714d1c4f19aecc" );
    assert ( Digest ( hashSet, Digest::HASH_ALGORITHM_MD5 ).toHex () == "9a8940fbb63927cb82d3a592d3eda2e4" );
    
    hashSet.clear ();
    hashSet.insert ( "c" );
    hashSet.insert ( "b" );
    hashSet.insert ( "a" );
    assert ( ToJSONSerializer::toDigestString ( hashSet ) == JSON_STR (["a","b","c"]));
    assert ( Digest ( hashSet, Digest::HASH_ALGORITHM_SHA256 ).toHex () == "fa1844c2988ad15ab7b49e0ece09684500fad94df916859fb9a43ff85f5bb477" );
    assert ( Digest ( hashSet, Digest::HASH_ALGORITHM_MD5 ).toHex () == "c29a5747d698b2f95cdfd5ed6502f19d" );

    SerializableMap < string, string > hashMap;

    hashMap [ "x" ] = "abc";
    assert ( ToJSONSerializer::toDigestString ( hashMap ) == JSON_STR ({"x":"abc"}));
    assert ( Digest ( hashMap, Digest::HASH_ALGORITHM_SHA256 ).toHex () == "270012bdffcdc54c226ac7d3bcc965b48e8c688c39b78313e40c82d96cda2dd4" );
    assert ( Digest ( hashMap, Digest::HASH_ALGORITHM_MD5 ).toHex () == "ae66cd8e799ad133fe404f068db1beb9" );

    hashMap.clear ();
    hashMap [ "z" ] = "c";
    hashMap [ "y" ] = "b";
    hashMap [ "x" ] = "a";
    assert ( ToJSONSerializer::toDigestString ( hashMap ) == JSON_STR ({"x":"a","y":"b","z":"c"}));
    assert ( Digest ( hashMap, Digest::HASH_ALGORITHM_SHA256 ).toHex () == "a37e1584a03d065fcd5320ded38b0e0091e037cbadcdaf10df8f55e0d60c823d" );
    assert ( Digest ( hashMap, Digest::HASH_ALGORITHM_MD5 ).toHex () == "ed8b3cf6a7023401f7416571e49d817a" );

    SerializableMap < string, u64 > hashMap64;
    hashMap64 [ "u32" ] = 0xffffffff;
    hashMap64 [ "u64" ] = ( u64 )0xffffffff + 1;
    hashMap64 [ "max" ] = ( u64 )0xffffffffffffffff;

    string hashMap64JSON = ToJSONSerializer::toDigestString ( hashMap64 );
    assert ( hashMap64JSON == JSON_STR ({"max":"0xffffffffffffffff","u32":4294967295,"u64":"0x100000000"}));
    assert ( Digest ( hashMap64, Digest::HASH_ALGORITHM_SHA256 ).toHex () == "66c456477effe884fcbededc2422e37dd6d77215f0a3f3702104ab1b0cee484f" );
    assert ( Digest ( hashMap64, Digest::HASH_ALGORITHM_MD5 ).toHex () == "4b58c72881bceaec0c6fc38cfb41ee9e" );

    hashMap64.clear ();
    FromJSONSerializer::fromJSONString ( hashMap64, hashMap64JSON );
    assert ( hashMap64 [ "u32" ] == 0xffffffff );
    assert ( hashMap64 [ "u64" ] == ( u64 )0xffffffff + 1 );
    assert ( hashMap64 [ "max" ] == ( u64 )0xffffffffffffffff );

    Block hashBlock;
    FromJSONSerializer::fromJSONString ( hashBlock, test_block_json );
    assert ( ToJSONSerializer::toDigestString ( hashBlock ) == JSON_STR ({"height":0,"time":0,"transactions":[]}));
    assert ( Digest ( hashBlock, Digest::HASH_ALGORITHM_SHA256 ).toHex () == "8f7383032c626071fde10fad55814c2107e4ed87f36e8370b36d10ad1f2870bc" );
    assert ( Digest ( hashBlock, Digest::HASH_ALGORITHM_MD5 ).toHex () == "f81606c250f6a0d55ba7c4f39bfea41f" );
    
    CryptoKey ellipticKey;
    ellipticKey.elliptic ( CryptoKey::DEFAULT_EC_GROUP_NID );
    assert ( MinerLaunchTests::checkDeterministic ( ellipticKey, 16, 16 ));
    
    Signature ellipticSigSHA256 = ellipticKey.sign ( "abc", Digest::HASH_ALGORITHM_SHA256 );
    assert ( ellipticKey.verify ( ellipticSigSHA256, "abc" ));
    assert ( ellipticKey.verify ( ellipticSigSHA256, "abcd" ) == false );
    
    Signature ellipticSigMD5 = ellipticKey.sign ( "abc", Digest::HASH_ALGORITHM_MD5 );
    assert ( ellipticKey.verify ( ellipticSigMD5, "abc" ));
    assert ( ellipticKey.verify ( ellipticSigMD5, "abcd" ) == false );
    
    CryptoKey rsaKey;
    rsaKey.rsa ( CryptoKey::RSA_1024 );
    assert ( MinerLaunchTests::checkDeterministic ( rsaKey, 16, 1 ));
    
    Signature rsaSigSHA256 = rsaKey.sign ( "abc", Digest::HASH_ALGORITHM_SHA256 );
    assert ( rsaKey.verify ( rsaSigSHA256, "abc" ));
    assert ( rsaKey.verify ( rsaSigSHA256, "abcd" ) == false );
    
    Signature rsaSigMD5 = rsaKey.sign ( "abc", Digest::HASH_ALGORITHM_MD5 );
    assert ( rsaKey.verify ( rsaSigMD5, "abc" ));
    assert ( rsaKey.verify ( rsaSigMD5, "abcd" ) == false );
}

} // namespace Volition
