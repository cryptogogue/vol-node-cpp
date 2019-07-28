// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractAPIRequestHandler.h>
#include <volition/FNV1a.h>

namespace Volition {

//================================================================//
// AbstractAPIRequestHandler
//================================================================//

//----------------------------------------------------------------//
AbstractAPIRequestHandler::AbstractAPIRequestHandler () {
}

//----------------------------------------------------------------//
AbstractAPIRequestHandler::~AbstractAPIRequestHandler () {
}

//----------------------------------------------------------------//
int AbstractAPIRequestHandler::getMethodForString ( string method ) {

    switch ( FNV1a::hash_64 ( method.c_str ())) {
        case FNV1a::const_hash_64 ( "DELETE" ):     return HTTP_DELETE;
        case FNV1a::const_hash_64 ( "GET" ):        return HTTP_GET;
        case FNV1a::const_hash_64 ( "HEAD" ):       return HTTP_HEAD;
        case FNV1a::const_hash_64 ( "OPTIONS" ):    return HTTP_OPTIONS;
        case FNV1a::const_hash_64 ( "PATCH" ):      return HTTP_PATCH;
        case FNV1a::const_hash_64 ( "POST" ):       return HTTP_POST;
        case FNV1a::const_hash_64 ( "PUT" ):        return HTTP_PUT;
    }
    return 0;
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void AbstractAPIRequestHandler::AbstractRequestHandler_handleRequest ( const Routing::PathMatch& match, Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response ) const {
    UNUSED ( match );

    response.add ( "Access-Control-Allow-Origin", "*" );
    response.add ( "Access-Control-Allow-Headers", "Accept, Content-Type, Origin, X-Requested-With" );
    response.add ( "Access-Control-Allow-Methods", "DELETE, GET, HEAD, OPTIONS, POST, PUT" );

    int method = AbstractAPIRequestHandler::getMethodForString ( request.getMethod ());
    
    if ( method == HTTP_OPTIONS ) {
        response.setStatus (  Poco::Net::HTTPResponse::HTTP_OK );
        response.send ();
        return;
    }
    
    if ( !( this->AbstractAPIRequestHandler_getSupportedHTTPMethods () & method )) {
        response.setStatus ( Poco::Net::HTTPResponse::HTTP_METHOD_NOT_ALLOWED );
        response.send ();
        return;
    }

    Poco::JSON::Object::Ptr jsonIn  = NULL;
    Poco::JSON::Object::Ptr jsonOut = new Poco::JSON::Object ();
    
    if ( method & ( HTTP_POST | HTTP_PUT )) {
    
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse ( request.stream ());
        jsonIn = result.extract < Poco::JSON::Object::Ptr >();
    
        if ( !jsonIn ) {
            response.setStatus ( Poco::Net::HTTPResponse::HTTP_BAD_REQUEST );
            response.send ();
            return;
        }
    }
    else {
        jsonIn = new Poco::JSON::Object ();
    }

    HTTPStatus status = this->AbstractAPIRequestHandler_handleRequest ( method, *jsonIn, *jsonOut );
    response.setStatus ( status );
    response.setContentType ( "application/json" );
    
    ostream& out = response.send ();
    jsonOut->stringify ( out, 4, -1 );
    out.flush ();
}

} // namespace Volition
