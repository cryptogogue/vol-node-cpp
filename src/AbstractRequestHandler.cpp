//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "AbstractRequestHandler.h"

namespace Volition {

//================================================================//
// AbstractRequestHandlerWithMatch
//================================================================//

//----------------------------------------------------------------//
AbstractRequestHandler::AbstractRequestHandler () {
}

//----------------------------------------------------------------//
AbstractRequestHandler::~AbstractRequestHandler () {
}

//----------------------------------------------------------------//
Poco::JSON::Object::Ptr AbstractRequestHandler::parseJSON ( Poco::Net::HTTPServerRequest &request ) {

    Poco::JSON::Parser parser;
    Poco::Dynamic::Var result = parser.parse ( request.stream ());
    return result.extract < Poco::JSON::Object::Ptr >();
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

