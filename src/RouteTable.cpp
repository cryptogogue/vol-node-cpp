//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "RouteTable.h"

namespace Volition {

//================================================================//
// AbstractEndpoint
//================================================================//

//----------------------------------------------------------------//
AbstractEndpoint::AbstractEndpoint () {
}

//----------------------------------------------------------------//
AbstractEndpoint::~AbstractEndpoint () {
}

//----------------------------------------------------------------//
AbstractRequestHandler* AbstractEndpoint::createRequestHandler ( const Routing::PathMatch& match ) const {

    AbstractRequestHandler* handler = this->AbstractEndpoint_createRequestHandler ();
    handler->setMatch ( match );
    return handler;
}

//================================================================//
// RouteTable
//================================================================//

//----------------------------------------------------------------//
AbstractRequestHandler* RouteTable::match ( string uri ) {

    try {
        Routing::PathMatch match = this->mRouter.matchPath ( uri );
        const string& pattern = match.pathTemplate ();

        if ( this->mPatternsToEndpoints.find ( pattern ) != this->mPatternsToEndpoints.end ()) {
            return this->mPatternsToEndpoints [ pattern ]->createRequestHandler ( match );
        }
    }
    catch ( Routing::PathNotFoundException ) {
    
        try {
            Routing::PathMatch match = this->mDefaultRouter.matchPath ( "" );
            return this->mDefaultEndpoint->createRequestHandler ( match );
        }
        catch ( Routing::PathNotFoundException ) {
        }
    }
    return 0;
}

//----------------------------------------------------------------//
RouteTable::RouteTable () {
}

//----------------------------------------------------------------//
RouteTable::~RouteTable () {
}

//----------------------------------------------------------------//
size_t RouteTable::size () {

    return this->mPatternsToEndpoints.size ();
}

} // namespace Volition

