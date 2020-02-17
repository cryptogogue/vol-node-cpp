// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AssetID.h>

// https://arxiv.org/html/0901.4016
// https://github.com/dsw/proquint

// AssetID isn't a proquint, but it plays one on TV. we're doing a couple of things
// differently. for example, going from low-to-high bits (vs. high-to-low, as in the proquint
// sample implementation). there is also an 8-bit checksum, for what it's worth. we don't
// anticipate people typing assets IDs much, but the checksum was easy enough to add. also, gives
// the asset names a sort of panache.

// we thought about truncating AssetIDs for lower numbered indices, but decided against it.
// it seems appealing to have all AssetIDs be the same length from the start.

// the AssetID itself is derived from an index, which is expected to be no more than 48 bits.
// by the time we need more than 48 bits (after Volition becomes self-aware and deploys the
// mind-control drones), Volition will have grown to absorb all data and computational resources
// in the known universe. can devise a new AssetID format then.

// "munge" and "spin" just exist to give the AssetIDs some variety. without them, the
// nature of AssetIDs as decorated counters would be plainly evident, and we'd be stuck with
// repeated letters for long stretches at a time. so munge/spin are purely aesthetic.

namespace Volition {

static const char sConsonants [] = {
    'b', 'd', 'f', 'g',
    'h', 'j', 'k', 'l',
    'm', 'n', 'p', 'r',
    's', 't', 'v', 'z'
};

static const char sVowels [] = {
    'a', 'i', 'o', 'u'
};

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
u8          _check              ( u64 number );
u16         _decode16           ( const char* buffer );
u8          _decodeConsonant    ( char vowel );
u8          _decodeVowel        ( char vowel );
void        _encode16           ( char* buffer, u16 number );
u64         _munge              ( u64 number );
u64         _spin               ( u64 number );
u64         _unmunge            ( u64 number );
u64         _unspin             ( u64 number );

//----------------------------------------------------------------//
u8 _check ( u64 number ) {

    u8 sum = 0;
    
    sum += number & 0xff;
    sum += ( number >> 8 ) & 0xff;
    sum += ( number >> 16 ) & 0xff;
    sum += ( number >> 24 ) & 0xff;
    sum += ( number >> 32 ) & 0xff;
    sum += ( number >> 40 ) & 0xff;
    
    return ~sum + 1;
}

//----------------------------------------------------------------//
u16 _decode16 ( const char* buffer ) {

    u16 number = 0;
    
    number |= _decodeConsonant  ( buffer [ 0 ]);
    number |= _decodeVowel      ( buffer [ 1 ]) << 4;
    number |= _decodeConsonant  ( buffer [ 2 ]) << 6;
    number |= _decodeVowel      ( buffer [ 3 ]) << 10;
    number |= _decodeConsonant  ( buffer [ 4 ]) << 12;

    return number;
}

//----------------------------------------------------------------//
u8 _decodeConsonant ( char vowel ) {

    switch ( vowel ) {
        case 'b':       return 0;
        case 'd':       return 1;
        case 'f':       return 2;
        case 'g':       return 3;
        case 'h':       return 4;
        case 'j':       return 5;
        case 'k':       return 6;
        case 'l':       return 7;
        case 'm':       return 8;
        case 'n':       return 9;
        case 'p':       return 10;
        case 'r':       return 11;
        case 's':       return 12;
        case 't':       return 13;
        case 'v':       return 14;
        case 'z':       return 15;
    }
    assert ( false );
    return 0;
}

//----------------------------------------------------------------//
u8 _decodeVowel ( char vowel ) {

    switch ( vowel ) {
        case 'a':       return 0;
        case 'i':       return 1;
        case 'o':       return 2;
        case 'u':       return 3;
    }
    assert ( false );
    return 0;
}

//----------------------------------------------------------------//
void _encode16 ( char* buffer, u16 number ) {

    buffer [ 0 ] = sConsonants  [ number & 0x0f ];
    buffer [ 1 ] = sVowels      [( number >> 4 ) & 0x03 ];
    buffer [ 2 ] = sConsonants  [( number >> 6 ) & 0x0f ];
    buffer [ 3 ] = sVowels      [( number >> 10 ) & 0x03 ];
    buffer [ 4 ] = sConsonants  [( number >> 12 ) & 0x0f ];
}

//----------------------------------------------------------------//
u64 _munge ( u64 number ) {

    u64 munge = number & 0x0f;
    return ((( number ^ sMungeTable [ munge ]) & ~(( u64 )0x0f )) | munge ) & FORTY_EIGHT_BITS;
}

//----------------------------------------------------------------//
u64 _spin ( u64 number ) {
    
    u64 spin = ( number >> 4 ) & 0x0f;
    u64 spinPortion = ( number >> 8 ) & FORTY_BITS; // 40-bits
    
    u64 spinUpper = ( spinPortion << spin ) & FORTY_BITS;
    u64 spinLower = ( spinPortion >> ( 40 - spin ));
    u64 spinFinal = spinUpper | spinLower;
    
    return number = ( spinFinal << 8 ) | ( number & 0xff );
}

//----------------------------------------------------------------//
u64 _unmunge ( u64 number ) {

    return _munge ( number );
}

//----------------------------------------------------------------//
u64 _unspin ( u64 number ) {

    u64 spin = ( number >> 4 ) & 0x0f;
    u64 spinPortion = ( number >> 8 ) & FORTY_BITS; // 40-bits
    
    u64 spinUpper = ( spinPortion << ( 40 - spin )) & FORTY_BITS;
    u64 spinLower = ( spinPortion >> spin );
    u64 spinFinal = spinUpper | spinLower;
    
    return number = ( spinFinal << 8 ) | ( number & 0xff );
}

//================================================================//
// AssetID
//================================================================//
namespace AssetID {

//----------------------------------------------------------------//
u64 decode ( string assetID, bool* isValid ) {

    // TODO: handle errors

    const char* buffer = assetID.c_str ();

    u64 index = 0;
    
    index |= ( u64 )_decode16 ( &buffer [ 0 ]);
    index |= (( u64 )_decode16 ( &buffer [ 6 ])) << 16;
    index |= (( u64 )_decode16 ( &buffer [ 12 ])) << 32;

    if ( isValid ) {
    
        int check = 0;
        check += ( buffer [ 18 ] - '0' ) * 100;
        check += ( buffer [ 19 ] - '0' ) * 10;
        check += buffer [ 20 ] - '0';

        *isValid = ( check == _check ( index ));
    }
    
    index = _unspin ( index );
    index = _unmunge ( index );
    
    return index;
}

//----------------------------------------------------------------//
string encode ( u64 index ) {

    assert (( index & ~FORTY_EIGHT_BITS ) == 0 );

    index = _munge ( index );
    index = _spin ( index );

    char buffer [ 24 ];
    memset ( buffer, 0, sizeof ( buffer ));

    buffer [ 5 ] = '-';
    buffer [ 11 ] = '-';
    buffer [ 17 ] = '-';

    _encode16 ( &buffer [ 0 ],  index & 0xffff );
    _encode16 ( &buffer [ 6 ],  ( index >> 16 ) & 0xffff );
    _encode16 ( &buffer [ 12 ], ( index >> 32 ) & 0xffff );

    buffer [ 18 ] = '0';

    int check = _check ( index );

    int dec = 100;
    for ( size_t i = 0; i < 3; ++i ) {
        buffer [ 18 + i ] = '0' + (( check / dec ) % 10 );
        dec = dec / 10;
    }
    
    return buffer;
}

} // namespace AssetID
} // namespace Volition
