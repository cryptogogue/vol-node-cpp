// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/RouteTable.h>

namespace Volition {

//================================================================//
// RouteTable
//================================================================//

//----------------------------------------------------------------//
AbstractRequestHandler* RouteTable::match ( string uri ) {

    unique_ptr < AbstractRequestHandler > handler;

    try {
        Routing::PathMatch match = this->mRouter.matchPath ( uri );
        const string& pattern = match.pathTemplate ();
        
        handler = this->create ( pattern );
        assert ( handler );
        handler->setMatch ( match );
    }
    catch ( Routing::PathNotFoundException ) {
    
        try {
            Routing::PathMatch match = this->mDefaultRouter.matchPath ( "" );
            handler = this->create ();
            assert ( handler );
            handler->setMatch ( match );
        }
        catch ( Routing::PathNotFoundException ) {
        }
    }
    return handler.release ();
}

//----------------------------------------------------------------//
RouteTable::RouteTable () {
}

//----------------------------------------------------------------//
RouteTable::~RouteTable () {
}

//----------------------------------------------------------------//
size_t RouteTable::size () {

    return this->getFactorySize ();
}

} // namespace Volition
