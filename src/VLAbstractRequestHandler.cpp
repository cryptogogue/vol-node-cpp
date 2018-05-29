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
VLAbstractRequestHandler::VLAbstractRequestHandler ( const PathMatch& match ) :
    mMatch ( match ) {
}

//----------------------------------------------------------------//
VLAbstractRequestHandler::~VLAbstractRequestHandler () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void VLAbstractRequestHandler::handleRequest ( HTTPServerRequest &request, HTTPServerResponse &response ) {

    this->VLAbstractRequestHandler_HandleRequest ( this->mMatch, request, response );
}

