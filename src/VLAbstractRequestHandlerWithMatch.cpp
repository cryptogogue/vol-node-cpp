//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "VLAbstractRequestHandlerWithMatch.h"

//================================================================//
// VLAbstractRequestHandlerWithMatch
//================================================================//

//----------------------------------------------------------------//
VLAbstractRequestHandlerWithMatch::VLAbstractRequestHandlerWithMatch () {
}

//----------------------------------------------------------------//
VLAbstractRequestHandlerWithMatch::~VLAbstractRequestHandlerWithMatch () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void VLAbstractRequestHandlerWithMatch::VLAbstractRequestHandler_HandleRequest ( HTTPServerRequest& request, HTTPServerResponse& response ) const {

    assert ( this->mMatch );
    this->VLAbstractRequestHandlerWithMatch_HandleRequest ( *this->mMatch, request, response );
}

//----------------------------------------------------------------//
void VLAbstractRequestHandlerWithMatch::VLAbstractRequestHandler_SetMatch ( const PathMatch& match ) {

    this->mMatch = make_unique < PathMatch >( match );
}
