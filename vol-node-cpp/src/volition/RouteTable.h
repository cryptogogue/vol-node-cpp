// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ROUTETABLE_H
#define VOLITION_ROUTETABLE_H

#include <volition/common.h>
#include <volition/AbstractRequestHandler.h>
#include <volition/Factory.h>
#include <volition/HTTP.h>

namespace Volition {

//================================================================//
// RouteTable
//================================================================//
template < typename REQUEST_HANDLER_TYPE >
class RouteTable :
    public Factory < REQUEST_HANDLER_TYPE > {
private:

    map < string, Factory < REQUEST_HANDLER_TYPE >> mFactories;
    unique_ptr < AbstractFactoryAllocator < REQUEST_HANDLER_TYPE >> mDefaultAllocator;
    
    Routing::Router             mRouter;
    Routing::Router             mDefaultRouter;

public:

    //----------------------------------------------------------------//
    unique_ptr < REQUEST_HANDLER_TYPE > match ( const Poco::Net::HTTPServerRequest& request ) {

        // make sure the request handler type is also a sublass of AbstractRequestHandler
        ( void )static_cast < AbstractRequestHandler* >(( REQUEST_HANDLER_TYPE* )NULL );

        string uri = request.getURI ();
        string uriNoQuery = uri.substr ( 0, uri.find ( '?' ));

        unique_ptr < REQUEST_HANDLER_TYPE > handler;

        try {
            Routing::PathMatch match = this->mRouter.matchPath ( uriNoQuery );
            const string& pattern = match.pathTemplate ();
            
            handler = this->mFactories [ request.getMethod ()].create ( pattern );
            
            if ( handler ) {
                
                handler->setMatch ( match );
                
                Poco::URI::QueryParameters queryParams = Poco::URI ( uri ).getQueryParameters ();
                Poco::URI::QueryParameters::const_iterator queryIt = queryParams.cbegin ();
                for ( ; queryIt != queryParams.cend (); ++queryIt ) {
                    handler->setQueryParam ( queryIt->first, queryIt->second );
                }
                return handler;
            }
        }
        catch ( Routing::PathNotFoundException ) {}
        
        try {
            Routing::PathMatch match = this->mDefaultRouter.matchPath ( "" );
            handler = this->mDefaultAllocator->create ();
            assert ( handler );
                    
            handler->setMatch ( match );
            return handler;
        }
        catch ( Routing::PathNotFoundException ) {}
        
        return NULL;
    }

    //----------------------------------------------------------------//
    RouteTable () {
    }

    //----------------------------------------------------------------//
    ~RouteTable () {
    }
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    void addEndpoint ( HTTP::Method methodMask, string pattern, shared_ptr < FactoryAllocator < AbstractRequestHandler, TYPE >> allocator = NULL ) {
    
        int key = 1;
        int cursor = ( int )methodMask;
        if ( cursor == 0 ) return;
        
        allocator = allocator ? allocator : make_shared < FactoryAllocator < AbstractRequestHandler, TYPE >>();
        
        for ( ; cursor; cursor >>= 1, key <<= 1 ) {
            if ( !( cursor & 1 )) continue;
            this->mFactories [ HTTP::getStringForMethod (( HTTP::Method )key )].template addFactoryAllocator < TYPE >( pattern );
        }
        this->mRouter.registerPath ( pattern );
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    void setDefault () {
        
        this->mDefaultAllocator = make_unique < FactoryAllocator < REQUEST_HANDLER_TYPE, TYPE >>();
        this->mDefaultRouter.registerPath ( "/?" );
    }
};

} // namespace Volition
#endif
