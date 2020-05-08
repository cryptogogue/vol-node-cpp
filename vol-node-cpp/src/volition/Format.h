// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_FORMAT_H
#define VOLITION_FORMAT_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// Format
//================================================================//
namespace Format {

    //----------------------------------------------------------------//
    time_t          fromISO8601         ( string iso8601 );
    string          toISO8601           ( time_t t );
    string          tolower             ( string str );
    string          toupper             ( string str );
    char*           vsnprintf_alloc     ( char* s, size_t n, const char* format, va_list arg );
    string          write               ( const char* format, ... );
    string          write_var           ( const char* format, va_list args );
    void            write               ( string& str, const char* format, ... );
    void            write_var           ( string& str, const char* format, va_list args );
};

} // namespace Volition
#endif
