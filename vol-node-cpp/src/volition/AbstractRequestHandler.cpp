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

    return stoll ( this->getMatchString ( key ));
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
void AbstractRequestHandler::setMatch ( const Routing::PathMatch& match ) {

    this->mMatch = make_unique < Routing::PathMatch >( match );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void AbstractRequestHandler::handleRequest ( Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response ) {

    this->AbstractRequestHandler_handleRequest ( *this->mMatch, request, response );
}

} // namespace Volition

