//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "VLAbstractRequestHandler.h"

//================================================================//
// VLAbstractRequestHandler
//================================================================//

//----------------------------------------------------------------//
void VLAbstractRequestHandler::SetMatch ( const PathMatch& match ) {

    this->VLAbstractRequestHandler_SetMatch ( match );
}

//----------------------------------------------------------------//
VLAbstractRequestHandler::VLAbstractRequestHandler () {
}

//----------------------------------------------------------------//
VLAbstractRequestHandler::~VLAbstractRequestHandler () {
}

//----------------------------------------------------------------//
void VLAbstractRequestHandler::VLAbstractRequestHandler_SetMatch ( const PathMatch& match ) {
    assert ( false );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void VLAbstractRequestHandler::handleRequest ( HTTPServerRequest& request, HTTPServerResponse& response ) {

    this->VLAbstractRequestHandler_HandleRequest ( request, response );
}

