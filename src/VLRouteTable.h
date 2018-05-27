#ifndef ROUTER_H
#define ROUTER_H

#include "common.h"

//================================================================//
// VLAbstractRequestHandler
//================================================================//
class VLAbstractRequestHandler :
	public HTTPRequestHandler {
public:

	//----------------------------------------------------------------//
					VLAbstractRequestHandler			();
					~VLAbstractRequestHandler			();
};

//================================================================//
// VLAbstractRequestHandlerWithMatch
//================================================================//
class VLAbstractRequestHandlerWithMatch :
	public VLAbstractRequestHandler {
private:

	friend class VLAbstractEndpoint;
	template < typename TYPE > friend class VLEndpoint;

	PathMatch		mMatch;

	//----------------------------------------------------------------//
	void			handleRequest								( HTTPServerRequest &request, HTTPServerResponse &response ) override;

protected:

	//----------------------------------------------------------------//
	virtual void	VLRequestHandler_HandleRequest				( const PathMatch& match, HTTPServerRequest &request, HTTPServerResponse &response ) = 0;

public:

	//----------------------------------------------------------------//
					VLAbstractRequestHandlerWithMatch			( const PathMatch& match );
					~VLAbstractRequestHandlerWithMatch			();
};

//================================================================//
// VLDefaultRequestHandler
//================================================================//
class VLDefaultRequestHandler :
	public VLAbstractRequestHandler {
private:

	//----------------------------------------------------------------//
	void			handleRequest						( HTTPServerRequest &request, HTTPServerResponse &response ) override;

public:

	//----------------------------------------------------------------//
					VLDefaultRequestHandler				();
					~VLDefaultRequestHandler			();
};

//================================================================//
// VLAbstractEndpoint
//================================================================//
class VLAbstractEndpoint {
protected:

	//----------------------------------------------------------------//
	virtual VLAbstractRequestHandler*		VLEndpointBase_CreateRequestHandler			( const PathMatch& match ) = 0;

public:

	//----------------------------------------------------------------//
	VLAbstractRequestHandler*				CreateRequestHandler			( const PathMatch& match );
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
	VLAbstractRequestHandler* VLEndpointBase_CreateRequestHandler ( const PathMatch& match ) override {
		VLAbstractRequestHandlerWithMatch* handler = new TYPE ( match );
		return handler;
	}
};

//================================================================//
// VLRouteTable
//================================================================//
class VLRouteTable {
private:

	Router									mRouter;
	map < string, VLAbstractEndpoint* >		mPatternsToEndpoints;

	//----------------------------------------------------------------//
	void					AddEndpoint					( string pattern, VLAbstractEndpoint* endpoint );

public:

	//----------------------------------------------------------------//
	VLAbstractRequestHandler*	Match						( string uri );
	size_t						Size						();
								VLRouteTable				();
								~VLRouteTable				();
	
	//----------------------------------------------------------------//
	template < typename TYPE >
	void AddEndpoint ( string pattern ) {
		this->AddEndpoint ( pattern, new VLEndpoint < TYPE >);
	}
};

#endif
