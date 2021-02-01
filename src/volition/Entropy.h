// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ENTROPY_H
#define VOLITION_ENTROPY_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// Entropy
//================================================================//
class Entropy {
private:

    static const size_t BUFFERSIZE = 16;

    u8     mBytes [ BUFFERSIZE ];
    
    //----------------------------------------------------------------//
    u8 hexToByte ( const char c ) {

        if (( c >= '0' ) && ( c <= '9' )) return ( u8 )( c - '0' );
        if (( c >= 'a' ) && ( c <= 'f' )) return ( u8 )( c + 10 - 'a' );
        if (( c >= 'A' ) && ( c <= 'F' )) return ( u8 )( c + 10 - 'A' );

        assert ( false );
        return 0xff;
    }
    
public:
    
    //----------------------------------------------------------------//
    void clear () {
        memset ( this->mBytes, 0, BUFFERSIZE );
    }
    
    //----------------------------------------------------------------//
    Entropy () {
        this->clear ();
    }
    
    //----------------------------------------------------------------//
    Entropy ( string hex ) {
    
        // start with a clear buffer.
        this->clear ();
        
        // bail if empty string.
        if ( hex.size () == 0 ) return;
        
        // inputs that don't have an even number of bytes are invalid.
        assert (( hex.size () & 0x01 ) == 0 );
        
        // get the raw bytes.
        const char* str = hex.c_str ();
        
        // since there are two characters for every byte (one char per 4-bit nibble), the result will be half the size of the input.
        size_t resultSize = hex.size () >> 1;
        
        // max size is the smaller of the result and the internal buffer size.
        size_t maxSize = resultSize < BUFFERSIZE ? resultSize : BUFFERSIZE;
        
        // step through the two buffers, decoding as we go.
        size_t inCursor = 0;
        for ( size_t resultCursor = 0; resultCursor < maxSize; ++resultCursor ) {
            
            // decode each nibble. string is a list of hex nibbles ordered [(hi,lo)(hi,lo)(hi,lo)].
            u8 hi = this->hexToByte ( str [ inCursor++ ]);
            u8 lo = this->hexToByte ( str [ inCursor++ ]);
            
            // nibble is 4 bits, so shift hi left by for and bitwise OR with lo to compose the byte.
            this->mBytes [ resultCursor ] = ( u8 )(( hi << 4 ) | lo );
        }
    }
    
    //----------------------------------------------------------------//
    u64 get ( size_t lowbit, size_t nBits ) {
    
        // entropy bits are padded with 0 at top.
        u64 result = 0;
    
        // if we want to sample any bits...
        if ( nBits ) {
            
            // starting byte is lowbit / 8.
            // if it is in the buffer...
            size_t startByte = lowbit >> 3;
            if ( startByte < BUFFERSIZE ) {
            
                // number of bits to shift each sample before adding it to the result.
                size_t resultCursor = 0;
                
                // check the starting bit within the byte. will be nonzero if not on a boundary.
                size_t startBit = lowbit % 8 ;
                
                // if we're not starting on a byte boundary...
                if ( startBit != 0 ) {
                    
                    // number of bits to sample from the staring byte *after* the start bit.
                    size_t sampleSize = 8 - startBit;
                    
                    // grab the upper part of the byte and step our starting byte forward.
                    // technically don't have to mask; shift right will pad with zeros.
                    result = ( this->mBytes [ startByte++ ] >> startBit );
                    
                    // result cursor and nBits change.
                    resultCursor += sampleSize;
                    nBits -= sampleSize;
                }
            
                // we'll sample the rest of out bytes based on the remaining bits.
                // sample is now byte-aligned (thanks to the business above).
                // we'll keep sampling til we hit the end of our buffer or until we've sampled all the requested bits.
                for ( size_t i = startByte; (( nBits > 0 ) && ( i < BUFFERSIZE )); ++i ) {
                
                    // if we're sampling a whole byte...
                    if ( nBits >= 8 ) {
                    
                        // take the whole byte, shifted up by the cursor;
                        result |= ( u64 )this->mBytes [ i ] << resultCursor;
                        
                        // step the cursor 8 bits forward.
                        resultCursor += 8;
                        
                        // and reduce the remaining bits to sample by 8.
                        nBits -= 8;
                    }
                    else {
                    
                        // this is the end... less than one byte remains to be sampled.
                        // in this one case we have to mask to get rid of anything that might be in the upper byte.
                        result |= (( u64 )this->mBytes [ i ] & (( 1 << nBits ) - 1 )) << resultCursor;
                        
                        // we don't have to do this, but in the interest of being thorough.
                        // and heck, might use it for error reporting later.
                        resultCursor += nBits;
                        
                        // this will terminate the for loop.
                        // could break here instead, but it's nice to end with accurate cursors.
                        nBits = 0;
                    }
                }
            }
        }
        return result;
    }
    
    //----------------------------------------------------------------//
    const u8* getBuffer () const {
    
        return this->mBytes;
    }
    
    //----------------------------------------------------------------//
    size_t getSize () const {
    
        return ( BUFFERSIZE * sizeof ( u8 ));
    }
};

} // namespace Volition
#endif
