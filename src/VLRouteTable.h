#ifndef VLROUTETABLE_H
#define VLROUTETABLE_H

#include "common.h"
#include "VLAbstractRequestHandler.h"

//================================================================//
// VLAbstractEndpoint
//================================================================//
class VLAbstractEndpoint {
protected:

    //----------------------------------------------------------------//
    virtual VLAbstractRequestHandler*       VLEndpointBase_CreateRequestHandler         () const = 0;

public:

    //----------------------------------------------------------------//
    VLAbstractRequestHandler*               CreateRequestHandler            ( const PathMatch& match ) const;
                                            VLAbstractEndpoint              ();
    virtual                                 ~VLAbstractEndpoint             ();
};

//================================================================//
// VLEndpoint
//================================================================//
template < typename TYPE >
class VLEndpoint :
    public VLAbstractEndpoint {
private:

    //----------------------------------------------------------------//
    VLAbstractRequestHandler* VLEndpointBase_CreateRequestHandler () const override {
        return new TYPE ();
    }
};

//================================================================//
// VLRouteTable
//================================================================//
class VLRouteTable {
private:

    Router                                                  mRouter;
    Router                                                  mDefaultRouter;
    map < string, unique_ptr < VLAbstractEndpoint > >       mPatternsToEndpoints;
    unique_ptr < VLAbstractEndpoint >                       mDefaultEndpoint;

public:

    //----------------------------------------------------------------//
    VLAbstractRequestHandler*   Match                       ( string uri );
    size_t                      Size                        ();
                                VLRouteTable                ();
                                ~VLRouteTable               ();
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    void AddEndpoint ( string pattern ) {
        this->mPatternsToEndpoints [ pattern ] = make_unique < VLEndpoint < TYPE > > ();
        this->mRouter.registerPath ( pattern );
    }
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    void SetDefault () {
        this->mDefaultEndpoint = make_unique < VLEndpoint < TYPE > > ();
        this->mDefaultRouter.registerPath ( "/?" );
    }
};

#endif
