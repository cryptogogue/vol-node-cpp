// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_FORMAT_H
#define VOLITION_FORMAT_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// Fromat
//================================================================//
namespace Format {

    //----------------------------------------------------------------//
    string          tolower             ( string str );
    string          toupper             ( string str );
    char*           vsnprintf_alloc     ( char* s, size_t n, const char* format, va_list arg );
    string          write               ( const char* format, ... );
    string          write_ver           ( const char* format, va_list args );
    void            write               ( string& str, const char* format, ... );
    void            write_var           ( string& str, const char* format, va_list args );
};

} // namespace Volition
#endif
