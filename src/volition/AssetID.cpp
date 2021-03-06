// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AssetID.h>
#include <volition/Munge.h>

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

static const u64 FORTY_EIGHT_BITS   = 0x0000ffffffffffff;

//----------------------------------------------------------------//
u8          _check              ( u64 number );
u16         _decode16           ( const char* buffer, bool& decodeError );
u8          _decodeConsonant    ( char consonant, bool& decodeError );
u8          _decodeVowel        ( char vowel, bool& decodeError );
void        _encode16           ( char* buffer, u16 number );

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
u16 _decode16 ( const char* buffer, bool& decodeError ) {

    u16 number = 0;
    
    number |= _decodeConsonant  ( buffer [ 0 ], decodeError );
    number |= _decodeVowel      ( buffer [ 1 ], decodeError ) << 4;
    number |= _decodeConsonant  ( buffer [ 2 ], decodeError ) << 6;
    number |= _decodeVowel      ( buffer [ 3 ], decodeError ) << 10;
    number |= _decodeConsonant  ( buffer [ 4 ], decodeError ) << 12;

    return number;
}

//----------------------------------------------------------------//
u8 _decodeConsonant ( char consonant, bool& decodeError ) {

    consonant = ( char )tolower ( consonant );

    switch ( consonant ) {
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
    decodeError = true;
    return 0;
}

//----------------------------------------------------------------//
u8 _decodeVowel ( char vowel, bool& decodeError ) {

    vowel = ( char )tolower ( vowel );

    switch ( vowel ) {
        case 'a':       return 0;
        case 'i':       return 1;
        case 'o':       return 2;
        case 'u':       return 3;
    }
    decodeError = true;
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

//================================================================//
// AssetID
//================================================================//

//----------------------------------------------------------------//
u64 AssetID::decode ( string assetID, bool* isValid ) {

    // TODO: handle errors

    char buffer [ 32 ];
    strncpy ( buffer, assetID.c_str (), sizeof ( buffer ));

    u64 index = 0;
    bool decodeError = false;
    
    index |= ( u64 )_decode16 ( &buffer [ 0 ], decodeError );
    index |= (( u64 )_decode16 ( &buffer [ 6 ], decodeError )) << 16;
    index |= (( u64 )_decode16 ( &buffer [ 12 ], decodeError )) << 32;

    int check = 0;
    check += ( buffer [ 18 ] - '0' ) * 100;
    check += ( buffer [ 19 ] - '0' ) * 10;
    check += buffer [ 20 ] - '0';
    
    bool valid = ( !decodeError && ( check == _check ( index )));
    
    if ( isValid ) {
        *isValid = valid;
    }
    
    if ( !valid ) return NULL_INDEX;
    
    index = Munge::unspin ( index );
    index = Munge::unmunge ( index );
    
    return index;
}

//----------------------------------------------------------------//
string AssetID::encode ( u64 index ) {

    assert (( index & ~FORTY_EIGHT_BITS ) == 0 );

    index = Munge::munge ( index );
    index = Munge::spin ( index );

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

} // namespace Volition
