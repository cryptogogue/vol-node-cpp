// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/common.h>

#include <sodium.h>



//----------------------------------------------------------------//
TEST ( SodiumSig, load ) {

    const char* MESSAGE         = "test";
    const size_t MESSAGE_LEN    = 4;

    unsigned char pk [ crypto_sign_PUBLICKEYBYTES ];
    unsigned char sk [ crypto_sign_SECRETKEYBYTES ];
    crypto_sign_keypair ( pk, sk );

    unsigned char signed_message [ crypto_sign_BYTES + MESSAGE_LEN ];
    unsigned long long signed_message_len;

    crypto_sign ( signed_message, &signed_message_len, ( const unsigned char* )MESSAGE, MESSAGE_LEN, sk );

    unsigned char unsigned_message [ MESSAGE_LEN ];
    unsigned long long unsigned_message_len;
    ASSERT_TRUE ( crypto_sign_open ( unsigned_message, &unsigned_message_len, signed_message, signed_message_len, pk ) == 0 );
}

//----------------------------------------------------------------//
TEST ( SodiumVerifySig, load ) {
    
    string message  = "hello";
    string pkeyHex  = "ecf005d687d15a3518e7e4a66ef6b7c06c3cb4533bb89f63e27559371854b6be";
    string sigHex   = "186ea118a47a781bd883f07e519918b944202e4239c496ed6a7ced00e5806977b1f534dda8d1ce11ca59546028623bb17a57b7b3a848c7304dd1e4ae0234990b68656c6c6f";

    unsigned char pkeyBin [ 256 ];
    size_t pkenBinLen = 0;
    
    unsigned char sigBin [ 256 ];
    size_t sigBinLen = 0;

    sodium_hex2bin ( pkeyBin, sizeof ( pkeyBin ), pkeyHex.c_str (), pkeyHex.size (), NULL, &pkenBinLen, NULL );
    sodium_hex2bin ( sigBin, sizeof ( sigBin ), sigHex.c_str (), sigHex.size (), NULL, &sigBinLen, NULL );

    ASSERT_TRUE ( pkenBinLen == crypto_sign_PUBLICKEYBYTES );
    ASSERT_TRUE (( sigBinLen - message.size ()) == crypto_sign_BYTES );

    unsigned char messageBin [ 256 ];
    unsigned long long messageBinLen = 0;

    ASSERT_TRUE ( crypto_sign_open ( messageBin, &messageBinLen, sigBin, sigBinLen, pkeyBin ) == 0 );
    
    messageBin [ messageBinLen ] = '\0';
    ASSERT_TRUE ( message == string (( char* )messageBin ));
}
