// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ROUTETABLE_H
#define VOLITION_ROUTETABLE_H

#include <volition/common.h>
#include <volition/AbstractRequestHandler.h>
#include <volition/Factory.h>

namespace Volition {

//================================================================//
// RouteTable
//================================================================//
class RouteTable :
    public Factory < AbstractRequestHandler > {
private:

    Routing::Router     mRouter;
    Routing::Router     mDefaultRouter;

public:

    //----------------------------------------------------------------//
    AbstractRequestHandler*     match                       ( string uri );
                                RouteTable                  ();
                                ~RouteTable                 ();
    size_t                      size                        ();
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    void addEndpoint ( string pattern ) {
        this->addFactoryAllocator < TYPE >( pattern );
        this->mRouter.registerPath ( pattern );
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    void setDefault () {
        this->setDefaultFactoryAllocator < TYPE >();
        this->mDefaultRouter.registerPath ( "/?" );
    }
};

} // namespace Volition
#endif
