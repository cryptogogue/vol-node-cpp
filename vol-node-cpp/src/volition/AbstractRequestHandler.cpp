// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractRequestHandler.h>

namespace Volition {

//================================================================//
// AbstractRequestHandler
//================================================================//

//----------------------------------------------------------------//
AbstractRequestHandler::AbstractRequestHandler () {
}

//----------------------------------------------------------------//
AbstractRequestHandler::~AbstractRequestHandler () {
}

//----------------------------------------------------------------//
string AbstractRequestHandler::getMatchString ( string key ) const {

    if ( this->mMatch ) {
        return ( *this->mMatch )[ key ];
    }
    throw Routing::NoSuchElementException ( "" );
}

//----------------------------------------------------------------//
u64 AbstractRequestHandler::getMatchU64 ( string key ) const {

    return stoull ( this->getMatchString ( key ));
}

//----------------------------------------------------------------//
string AbstractRequestHandler::getQueryParamString ( string key ) const {

    map < string, string >::const_iterator queryIt = this->mQuery.find ( key );
    if ( queryIt != this->mQuery.cend ()) {
        return queryIt->second;
    }
    throw Routing::NoSuchElementException ( "" );
}

//----------------------------------------------------------------//
u64 AbstractRequestHandler::getQueryParamU64 ( string key ) const {

    return stoull ( this->getQueryParamString ( key ));
}

//----------------------------------------------------------------//
bool AbstractRequestHandler::hasQueryParam ( string key ) const {

    return ( this->mQuery.find ( key ) != this->mQuery.cend ());
}

//----------------------------------------------------------------//
u64 AbstractRequestHandler::optMatch ( string key, u64 fallback ) const {

    if ( this->mMatch ) {
        try {
           return this->getMatchU64 ( key );
        }
        catch ( ... ) {
        }
    }
    return fallback;
}

//----------------------------------------------------------------//
string AbstractRequestHandler::optMatch ( string key, string fallback ) const {

    if ( this->mMatch ) {
        try {
            return this->getMatchString ( key );
        }
        catch ( ... ) {
        }
    }
    return fallback;
}

//----------------------------------------------------------------//
u64 AbstractRequestHandler::optQuery ( string key, u64 fallback ) const {

    return this->hasQueryParam ( key ) ? this->getQueryParamU64 ( key ) : fallback;
}

//----------------------------------------------------------------//
string AbstractRequestHandler::optQuery ( string key, string fallback ) const {

    return this->hasQueryParam ( key ) ? this->getQueryParamString ( key ) : fallback;
}

//----------------------------------------------------------------//
void AbstractRequestHandler::setMatch ( const Routing::PathMatch& match ) {

    this->mMatch = make_unique < Routing::PathMatch >( match );
}

//----------------------------------------------------------------//
void AbstractRequestHandler::setQueryParam ( string name, string value ) {

    this->mQuery [ name ] = value;
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void AbstractRequestHandler::handleRequest ( Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response ) {

    this->AbstractRequestHandler_handleRequest ( *this->mMatch, request, response );
}

} // namespace Volition

