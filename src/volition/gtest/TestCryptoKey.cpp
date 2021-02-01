// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/CryptoKey.h>

using namespace Volition;

//----------------------------------------------------------------//
TEST ( CryptoKeyPair, load ) {
    
    CryptoKeyPair keyPair;
    
    keyPair.load ( "./test/keys/test-ec.priv.json" );
    ASSERT_TRUE ( keyPair );
    
    keyPair.load ( "./test/keys/test-ec.priv.pem" );
    ASSERT_TRUE ( keyPair );
    
    keyPair.load ( "./test/keys/test-rsa.priv.json" );
    ASSERT_TRUE ( keyPair );
    
    keyPair.load ( "./test/keys/test-rsa.priv.pem" );
    ASSERT_TRUE ( keyPair );
}

//----------------------------------------------------------------//
TEST ( CryptoPublicKey, load ) {
        
    CryptoPublicKey pubKey;
    
    pubKey.load ( "./test/keys/test-ec.pub.json" );
    ASSERT_TRUE ( pubKey );
    
    pubKey.load ( "./test/keys/test-ec.pub.pem" );
    ASSERT_TRUE ( pubKey );
    
    pubKey.load ( "./test/keys/test-rsa.pub.json" );
    ASSERT_TRUE ( pubKey );
    
    pubKey.load ( "./test/keys/test-rsa.pub.pem" );
    ASSERT_TRUE ( pubKey );
}
