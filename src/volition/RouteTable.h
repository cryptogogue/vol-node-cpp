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
class RouteTable :
    public Factory < AbstractRequestHandler > {
private:

    map < string, Factory < AbstractRequestHandler >> mFactories;
    unique_ptr < AbstractFactoryAllocator < AbstractRequestHandler >> mDefaultAllocator;
    
    Routing::Router             mRouter;
    Routing::Router             mDefaultRouter;

public:

    //----------------------------------------------------------------//
    AbstractRequestHandler*     match                       ( const Poco::Net::HTTPServerRequest& request );
                                RouteTable                  ();
                                ~RouteTable                 ();
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    void addEndpoint ( HTTP::Method methodMask, string pattern ) {
    
        int key = 1;
        int cursor = ( int )methodMask;
        for ( ; cursor; cursor >>= 1, key <<= 1 ) {
            if ( !( cursor & 1 )) continue;
            this->mFactories [ HTTP::getStringForMethod (( HTTP::Method )key )].addFactoryAllocator < TYPE >( pattern );
        }
        this->mRouter.registerPath ( pattern );
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    void setDefault () {
        this->mDefaultAllocator = make_unique < FactoryAllocator < AbstractRequestHandler, TYPE >>();
        this->mDefaultRouter.registerPath ( "/?" );
    }
};

} // namespace Volition
#endif
