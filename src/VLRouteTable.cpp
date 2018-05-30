//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "VLRouteTable.h"

//================================================================//
// VLAbstractEndpoint
//================================================================//

//----------------------------------------------------------------//
VLAbstractRequestHandler* VLAbstractEndpoint::CreateRequestHandler ( const PathMatch& match ) const {

    VLAbstractRequestHandler* handler = this->VLEndpointBase_CreateRequestHandler ();
    handler->SetMatch ( match );
    return handler;
}

//----------------------------------------------------------------//
VLAbstractEndpoint::VLAbstractEndpoint () {
}

//----------------------------------------------------------------//
VLAbstractEndpoint::~VLAbstractEndpoint () {
}

//================================================================//
// VLRouteTable
//================================================================//

//----------------------------------------------------------------//
VLAbstractRequestHandler* VLRouteTable::Match ( string uri ) {

    try {
        PathMatch match = this->mRouter.matchPath ( uri );
        const string& pattern = match.pathTemplate ();

        if ( this->mPatternsToEndpoints.find ( pattern ) != this->mPatternsToEndpoints.end ()) {
            return this->mPatternsToEndpoints [ pattern ]->CreateRequestHandler ( match );
        }
    }
    catch ( PathNotFoundException ) {
    
        try {
            PathMatch match = this->mDefaultRouter.matchPath ( "" );
            return this->mDefaultEndpoint->CreateRequestHandler ( match );
        }
        catch ( PathNotFoundException ) {
        }
    }
    return 0;
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
}
