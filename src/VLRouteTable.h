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
	virtual VLAbstractRequestHandler*		VLEndpointBase_CreateRequestHandler			( const PathMatch& match ) const = 0;

public:

	//----------------------------------------------------------------//
	VLAbstractRequestHandler*				CreateRequestHandler			( const PathMatch& match ) const;
											VLAbstractEndpoint				();
	virtual									~VLAbstractEndpoint				();
};

//================================================================//
// VLEndpoint
//================================================================//
template < typename TYPE >
class VLEndpoint :
	public VLAbstractEndpoint {
private:

	//----------------------------------------------------------------//
	VLAbstractRequestHandler* VLEndpointBase_CreateRequestHandler ( const PathMatch& match ) const override {
		VLAbstractRequestHandler* handler = new TYPE ( match );
		return handler;
	}
};

//================================================================//
// VLRouteTable
//================================================================//
class VLRouteTable {
private:

	Router													mRouter;
	map < string, unique_ptr < VLAbstractEndpoint > >		mPatternsToEndpoints;

public:

	//----------------------------------------------------------------//
	VLAbstractRequestHandler*	Match						( string uri );
	size_t						Size						();
								VLRouteTable				();
								~VLRouteTable				();
	
	//----------------------------------------------------------------//
	template < typename TYPE >
	void AddEndpoint ( string pattern ) {
		this->mPatternsToEndpoints [ pattern ] = make_unique < VLEndpoint < TYPE > > ();
		this->mRouter.registerPath ( pattern );
	}
};

#endif
