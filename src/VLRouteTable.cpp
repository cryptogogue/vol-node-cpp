//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "VLRouteTable.h"

//================================================================//
// VLAbstractRequestHandler
//================================================================//

//----------------------------------------------------------------//
VLAbstractRequestHandler::VLAbstractRequestHandler () {
}

//----------------------------------------------------------------//
VLAbstractRequestHandler::~VLAbstractRequestHandler () {
}

//================================================================//
// VLAbstractRequestHandlerWithMatch
//================================================================//

//----------------------------------------------------------------//
void VLAbstractRequestHandlerWithMatch::handleRequest ( HTTPServerRequest &request, HTTPServerResponse &response ) {

	this->VLRequestHandler_HandleRequest ( this->mMatch, request, response );
}

//----------------------------------------------------------------//
VLAbstractRequestHandlerWithMatch::VLAbstractRequestHandlerWithMatch ( const PathMatch& match ) :
	mMatch ( match ) {
}

//----------------------------------------------------------------//
VLAbstractRequestHandlerWithMatch::~VLAbstractRequestHandlerWithMatch () {
}

//================================================================//
// VLAbstractEndpoint
//================================================================//

//----------------------------------------------------------------//
VLAbstractRequestHandler* VLAbstractEndpoint::CreateRequestHandler ( const PathMatch& match ) {

	printf ( "VLAbstractEndpoint::CreateRequestHandler\n" );

	return this->VLEndpointBase_CreateRequestHandler ( match );
}

//----------------------------------------------------------------//
VLAbstractEndpoint::VLAbstractEndpoint () {
}

//----------------------------------------------------------------//
VLAbstractEndpoint::~VLAbstractEndpoint () {
}

//================================================================//
// VLDefaultRequestHandler
//================================================================//

//----------------------------------------------------------------//
void VLDefaultRequestHandler::VLDefaultRequestHandler::handleRequest ( HTTPServerRequest &request, HTTPServerResponse &response ) {
	response.setStatus ( HTTPResponse::HTTP_OK );
	response.setContentType ( "text/html" );

	ostream& out = response.send ();
	out << "<h1>DEFAULT HANDLER!</h1>";
	out.flush ();
}

//----------------------------------------------------------------//
VLDefaultRequestHandler::VLDefaultRequestHandler::VLDefaultRequestHandler () {
}

//----------------------------------------------------------------//
VLDefaultRequestHandler::VLDefaultRequestHandler::~VLDefaultRequestHandler () {
}

//================================================================//
// VLRouteTable
//================================================================//

//----------------------------------------------------------------//
void VLRouteTable::AddEndpoint ( string pattern, VLAbstractEndpoint* endpoint ) {

	assert ( endpoint );

	if ( this->mPatternsToEndpoints.find ( pattern ) != this->mPatternsToEndpoints.end ()) {
		delete this->mPatternsToEndpoints [ pattern ];
	}

	this->mPatternsToEndpoints [ pattern ] = endpoint;
	
	this->mRouter.registerPath ( pattern );
}

//----------------------------------------------------------------//
VLAbstractRequestHandler* VLRouteTable::Match ( string uri ) {

	printf ( "VLRouteTable::Match\n" );

	try {

		PathMatch match = this->mRouter.matchPath ( uri );
		const string& pattern = match.pathTemplate ();

		printf ( "match template: %s\n", pattern.c_str ());

		if ( this->mPatternsToEndpoints.find ( pattern ) != this->mPatternsToEndpoints.end ()) {

			printf ( "FOUND MATCH\n" );

			VLAbstractEndpoint* endPoint = this->mPatternsToEndpoints [ pattern ];
			return endPoint->CreateRequestHandler ( match );
		}
	}
	catch ( PathNotFoundException ) {
	}
	
	printf ( "NO MATCH\n" );
	return new VLDefaultRequestHandler ();
}

//----------------------------------------------------------------//
size_t VLRouteTable::Size () {

	return this->mPatternsToEndpoints.size ();
}

//----------------------------------------------------------------//
VLRouteTable::VLRouteTable () {
}

//----------------------------------------------------------------//
VLRouteTable::~VLRouteTable () {

	map < string, VLAbstractEndpoint* >::iterator endpointIt = this->mPatternsToEndpoints.begin ();
	for ( ; endpointIt != this->mPatternsToEndpoints.end (); ++endpointIt ) {
		delete endpointIt->second;
	}
	this->mPatternsToEndpoints.clear ();
}
