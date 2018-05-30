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

    //----------------------------------------------------------------//
    void            handleRequest                               ( HTTPServerRequest& request, HTTPServerResponse& response ) override;

protected:

    //----------------------------------------------------------------//
    virtual void    VLAbstractRequestHandler_HandleRequest      ( HTTPServerRequest& request, HTTPServerResponse& response ) const = 0;
    virtual void    VLAbstractRequestHandler_SetMatch           ( const PathMatch& match );

public:

    //----------------------------------------------------------------//
    void            SetMatch                            ( const PathMatch& match );
                    VLAbstractRequestHandler            ();
                    ~VLAbstractRequestHandler           ();
};

#endif
