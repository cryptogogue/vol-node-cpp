// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Munge.h>

namespace Volition {

//================================================================//
// Munge
//================================================================//
namespace Munge {

static const u64 sMungeTable [] = {
    0xa5e4693eafb7d5a8,
    0xc13cde4a4bed3c26,
    0xd0a4312f5784f20b,
    0x87c664b10f1b7f5d,
    0x2df6089b2178736e,
    0xd60e483423994989,
    0x676f404e617efac2,
    0xccf470d16ede17cf,
    0x4d0e5d9bcb5a8d82,
    0x3102b11a691398f2,
    0x5bb0a1275a0dfed9,
    0x811f76a6ca6acb4f,
    0x5f2edfa280bfd73a,
    0xe4d67e7011f01c65,
    0xe5c40e1ceaaac378,
    0x6b207c44e4b46d40,
};

static const u64 FORTY_BITS         = 0x000000ffffffffff;
static const u64 FORTY_EIGHT_BITS   = 0x0000ffffffffffff;

//----------------------------------------------------------------//
u64 munge ( u64 number ) {

    u64 munge = number & 0x0f;
    return ((( number ^ sMungeTable [ munge ]) & ~(( u64 )0x0f )) | munge ) & FORTY_EIGHT_BITS;
}

//----------------------------------------------------------------//
u64 spin ( u64 number ) {
    
    u64 spin = ( number >> 4 ) & 0x0f;
    u64 spinPortion = ( number >> 8 ) & FORTY_BITS; // 40-bits
    
    u64 spinUpper = ( spinPortion << spin ) & FORTY_BITS;
    u64 spinLower = ( spinPortion >> ( 40 - spin ));
    u64 spinFinal = spinUpper | spinLower;
    
    return number = ( spinFinal << 8 ) | ( number & 0xff );
}

//----------------------------------------------------------------//
u64 unmunge ( u64 number ) {

    return munge ( number );
}

//----------------------------------------------------------------//
u64 unspin ( u64 number ) {

    u64 spin = ( number >> 4 ) & 0x0f;
    u64 spinPortion = ( number >> 8 ) & FORTY_BITS; // 40-bits
    
    u64 spinUpper = ( spinPortion << ( 40 - spin )) & FORTY_BITS;
    u64 spinLower = ( spinPortion >> spin );
    u64 spinFinal = spinUpper | spinLower;
    
    return number = ( spinFinal << 8 ) | ( number & 0xff );
}

} // namespace AssetID
} // namespace Volition
