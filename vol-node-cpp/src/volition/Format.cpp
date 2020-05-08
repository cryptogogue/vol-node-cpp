// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Format.h>

namespace Volition {

int     to_lower_int    ( int _c );
int     to_upper_int    ( int _c );

//----------------------------------------------------------------//
int to_lower_int ( int _c ) {
    return ( toupper ( _c ));
}

//----------------------------------------------------------------//
int to_upper_int ( int _c ) {
    return ( toupper ( _c ));
}

//================================================================//
// Format
//================================================================//
namespace Format {

//----------------------------------------------------------------//
time_t fromISO8601 ( string iso8601 ) {

    if ( iso8601.size ()) {
    
        int tzd;
        Poco::DateTime dt;
        Poco::DateTimeParser ().parse ( Poco::DateTimeFormat::ISO8601_FORMAT, iso8601, dt, tzd );
        dt.makeUTC ( tzd );
    
        return dt.timestamp ().epochTime ();
    }
    return 0;
}

//----------------------------------------------------------------//
string toISO8601 ( time_t t ) {

    string iso8601 = Poco::DateTimeFormatter ().format ( Poco::Timestamp ().fromEpochTime ( t ), Poco::DateTimeFormat::ISO8601_FORMAT );
    return iso8601;
}

//----------------------------------------------------------------//
string tolower ( string str ) {

    transform ( str.begin (), str.end (), str.begin (), ( char( * )( char ))to_lower_int );
    return str;
}

//----------------------------------------------------------------//
string toupper ( string str ) {

    transform ( str.begin (), str.end (), str.begin (), ( char( * )( char ))to_upper_int );
    return str;
}

//----------------------------------------------------------------//
char* vsnprintf_alloc ( char* s, size_t n, const char* format, va_list arg ) {

    char* buffer = s;
    size_t buffSize = n;
    
    size_t result;
    
    while ( buffer ) {
        
        va_list copy;
        va_copy ( copy, arg );
        result = ( size_t )vsnprintf ( buffer, buffSize, format, copy );
        va_end ( copy );

        // thanks to http://perfec.to/vsnprintf/ for a discussion of vsnprintf portability issues
        if (( result == buffSize ) || ( result == ( size_t )-1 ) || ( result == buffSize - 1 ))  {
            buffSize = buffSize << 1;
        }
        else if ( result > buffSize ) {
            buffSize = result + 2;
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
string write ( const char* format, ... ) {
    
    string str;
    
    va_list args;
    va_start ( args, format );
    write_var ( str, format, args );
    va_end ( args );
    
    return str;
}

//----------------------------------------------------------------//
string write_var ( const char* format, va_list args ) {

    string str;
    write_var ( str, format, args );
    return str;
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
