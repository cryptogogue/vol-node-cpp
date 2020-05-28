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
            
            printf ( "FOUND HANDLER\n" );
            
            handler->setMatch ( match );
            
            Poco::URI::QueryParameters queryParams = Poco::URI ( uri ).getQueryParameters ();
            Poco::URI::QueryParameters::const_iterator queryIt = queryParams.cbegin ();
            for ( ; queryIt != queryParams.cend (); ++queryIt ) {
                handler->setQueryParam ( queryIt->first, queryIt->second );
            }
            return handler.release ();
        }
    }
    catch ( Routing::PathNotFoundException ) {
    
        printf ( "Routing::PathNotFoundException\n" );
    }
    
    printf ( "NO HANDLER FOUND\n" );
    
    try {
        Routing::PathMatch match = this->mDefaultRouter.matchPath ( "" );
        handler = this->mDefaultAllocator->create ();
        assert ( handler );
        
        printf ( "FOUND DEFAULT HANDLER\n" );
        
        handler->setMatch ( match );
        return handler.release ();
    }
    catch ( Routing::PathNotFoundException ) {
    
        printf ( "Routing::PathNotFoundException (DEFAULT HANDLER )\n" );
    }
    
    return NULL;
}

//----------------------------------------------------------------//
RouteTable::RouteTable () {
}

//----------------------------------------------------------------//
RouteTable::~RouteTable () {
}

} // namespace Volition

