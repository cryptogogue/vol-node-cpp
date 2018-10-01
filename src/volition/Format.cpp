// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Format.h>

namespace Volition {

//================================================================//
// Format
//================================================================//
namespace Format {

//----------------------------------------------------------------//
char* vsnprintf_alloc ( char* s, size_t n, const char* format, va_list arg ) {

    char* buffer = s;
    size_t buffSize = n;
    
    int result;
    
    while ( buffer ) {
        
        va_list copy;
        va_copy ( copy, arg );
        result = vsnprintf ( buffer, buffSize, format, copy );
        va_end ( copy );

        // thanks to http://perfec.to/vsnprintf/ for a discussion of vsnprintf portability issues
        if (( result == buffSize ) || ( result == -1 ) || ( result == buffSize - 1 ))  {
            buffSize = buffSize << 1;
        }
        else if ( result > buffSize ) {
            buffSize = ( size_t )result + 2;
        }
        else {
            break;
        }
        
        if ( buffer == s ) {
            buffer = 0;
        }
        
        if ( buffer ) {
            
            char* bufferRealloc = ( char* )realloc ( buffer, buffSize );
            if ( !bufferRealloc ) {
                free ( buffer );
                buffer = 0;
            }
            else {
                buffer = bufferRealloc;
            }
        }
        else {
            buffer = ( char* )malloc ( buffSize );
        }
        
        assert ( buffer );
    }

    return buffer;
}

//----------------------------------------------------------------//
void write ( string& str, const char* format, ... ) {

    va_list args;
    va_start ( args, format );
    
    write_var ( str, format, args );
    
    va_end ( args );
}

//----------------------------------------------------------------//
void write_var ( string& str, const char* format, va_list args ) {

    char buffer [ 1024 ];
    char* result = vsnprintf_alloc ( buffer, sizeof ( buffer ), format, args );
    
    str.append ( result );
    
    if ( buffer != result ) {
        free ( result );
    }
}

} // namespace Format
} // namespace Volition
