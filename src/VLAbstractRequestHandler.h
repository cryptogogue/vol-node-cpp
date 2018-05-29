#ifndef VLABSTRACTREQUESTHANDLER_H
#define VLABSTRACTREQUESTHANDLER_H

#include "common.h"

//================================================================//
// VLAbstractRequestHandler
//================================================================//
class VLAbstractRequestHandler :
    public HTTPRequestHandler {
private:

    friend class VLAbstractEndpoint;
    template < typename TYPE > friend class VLEndpoint;

    PathMatch       mMatch;

    //----------------------------------------------------------------//
    void            handleRequest                               ( HTTPServerRequest &request, HTTPServerResponse &response ) override;

protected:

    //----------------------------------------------------------------//
    virtual void    VLAbstractRequestHandler_HandleRequest      ( const PathMatch& match, HTTPServerRequest &request, HTTPServerResponse &response ) const = 0;

public:

    //----------------------------------------------------------------//
                    VLAbstractRequestHandler            ( const PathMatch& match );
                    ~VLAbstractRequestHandler           ();
};

#endif
