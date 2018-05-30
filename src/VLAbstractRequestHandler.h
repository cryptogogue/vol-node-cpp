#ifndef VLABSTRACTREQUESTHANDLER_H
#define VLABSTRACTREQUESTHANDLER_H

#include "common.h"

//================================================================//
// VLAbstractRequestHandler
//================================================================//
class VLAbstractRequestHandler :
    public HTTPRequestHandler {
private:
    
    unique_ptr < PathMatch > mMatch;

    //----------------------------------------------------------------//
    void            handleRequest                               ( HTTPServerRequest& request, HTTPServerResponse& response ) override;

protected:

    //----------------------------------------------------------------//
    virtual void    VLAbstractRequestHandler_HandleRequest      ( const PathMatch& match, HTTPServerRequest &request, HTTPServerResponse &response ) const = 0;

public:

    //----------------------------------------------------------------//
    void            SetMatch                                    ( const PathMatch& match );
                    VLAbstractRequestHandler                    ();
                    ~VLAbstractRequestHandler                   ();
};

#endif
