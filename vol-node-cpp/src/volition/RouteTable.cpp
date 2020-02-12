// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/RouteTable.h>

namespace Volition {

//================================================================//
// RouteTable
//================================================================//

//----------------------------------------------------------------//
AbstractRequestHandler* RouteTable::match ( const Poco::Net::HTTPServerRequest& request ) {

    string uri = request.getURI ();

    unique_ptr < AbstractRequestHandler > handler;

    try {
        Routing::PathMatch match = this->mRouter.matchPath ( uri );
        const string& pattern = match.pathTemplate ();
        
        handler = this->mFactories [ request.getMethod ()].create ( pattern );
        if ( handler ) {
            handler->setMatch ( match );
            return handler.release ();
        }
    }
    catch ( Routing::PathNotFoundException ) {}
    
    try {
        Routing::PathMatch match = this->mDefaultRouter.matchPath ( "" );
        handler = this->mDefaultAllocator->create ();
        assert ( handler );
        handler->setMatch ( match );
        return handler.release ();
    }
    catch ( Routing::PathNotFoundException ) {}
    
    return NULL;
}

//----------------------------------------------------------------//
RouteTable::RouteTable () {
}

//----------------------------------------------------------------//
RouteTable::~RouteTable () {
}

} // namespace Volition

