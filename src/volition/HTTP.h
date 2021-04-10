// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_HTTP_H
#define VOLITION_HTTP_H

#include <volition/common.h>
#include <volition/FNV1a.h>

namespace Volition {

//================================================================//
// HTTP
//================================================================//
class HTTP {
public:
    
    enum Method {
        ALL             = 0x7f,
        DELETE          =  1 << 0,
        GET             =  1 << 1,
        HEAD            =  1 << 2,
        OPTIONS         =  1 << 3,
        PATCH           =  1 << 4,
        POST            =  1 << 5,
        PUT             =  1 << 6,
        UNKNOWN         = 0,
    };
    
    static const Method GET_POST    = ( Method )( GET | POST );
    static const Method GET_PUT     = ( Method )( GET | PUT );
    
    //----------------------------------------------------------------//
    static HTTP::Method getMethodForString ( string method ) {
        switch ( FNV1a::hash_64 ( method.c_str ())) {
            case FNV1a::const_hash_64 ( "DELETE" ):     return DELETE;
            case FNV1a::const_hash_64 ( "GET" ):        return GET;
            case FNV1a::const_hash_64 ( "HEAD" ):       return HEAD;
            case FNV1a::const_hash_64 ( "OPTIONS" ):    return OPTIONS;
            case FNV1a::const_hash_64 ( "PATCH" ):      return PATCH;
            case FNV1a::const_hash_64 ( "POST" ):       return POST;
            case FNV1a::const_hash_64 ( "PUT" ):        return PUT;
        }
        return UNKNOWN;
    }
    
    //----------------------------------------------------------------//
    static string getStringForMethod ( HTTP::Method method ) {
        switch ( method ) {
            case DELETE:        return "DELETE";
            case GET:           return "GET";
            case HEAD:          return "HEAD";
            case OPTIONS:       return "OPTIONS";
            case PATCH:         return "PATCH";
            case POST:          return "POST";
            case PUT:           return "PUT";
            default:
                break;
        }
        return "";
    }
};

} // namespace Volition
#endif
