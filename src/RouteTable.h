#ifndef VLROUTETABLE_H
#define VLROUTETABLE_H

#include "common.h"
#include "AbstractRequestHandler.h"

namespace Volition {

//================================================================//
// AbstractEndpoint
//================================================================//
class AbstractEndpoint {
protected:

    //----------------------------------------------------------------//
    virtual AbstractRequestHandler*         AbstractEndpoint_createRequestHandler       () const = 0;

public:

    //----------------------------------------------------------------//
    AbstractRequestHandler*                 createRequestHandler            ( const Routing::PathMatch& match ) const;
                                            AbstractEndpoint                ();
    virtual                                 ~AbstractEndpoint               ();
};

//================================================================//
// Endpoint
//================================================================//
template < typename TYPE >
class Endpoint :
    public AbstractEndpoint {
private:

    //----------------------------------------------------------------//
    AbstractRequestHandler* AbstractEndpoint_createRequestHandler () const override {
        return new TYPE ();
    }
};

//================================================================//
// RouteTable
//================================================================//
class RouteTable {
private:

    Routing::Router                                         mRouter;
    Routing::Router                                         mDefaultRouter;
    map < string, unique_ptr < AbstractEndpoint >>          mPatternsToEndpoints;
    unique_ptr < AbstractEndpoint >                         mDefaultEndpoint;

public:

    //----------------------------------------------------------------//
    AbstractRequestHandler*     match                       ( string uri );
                                RouteTable                  ();
                                ~RouteTable                 ();
    size_t                      size                        ();
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    void addEndpoint ( string pattern ) {
        this->mPatternsToEndpoints [ pattern ] = make_unique < Endpoint < TYPE >> ();
        this->mRouter.registerPath ( pattern );
    }
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    void setDefault () {
        this->mDefaultEndpoint = make_unique < Endpoint < TYPE >> ();
        this->mDefaultRouter.registerPath ( "/?" );
    }
};

} // namespace Volition
#endif
