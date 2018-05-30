#ifndef VLABSTRACTREQUESTHANDLERWITHMATCH_H
#define VLABSTRACTREQUESTHANDLERWITHMATCH_H

#include "common.h"
#include <VLAbstractRequestHandler.h>

//================================================================//
// VLAbstractRequestHandlerWithMatch
//================================================================//
class VLAbstractRequestHandlerWithMatch :
    public VLAbstractRequestHandler {
private:
    
    unique_ptr < PathMatch > mMatch;

    //----------------------------------------------------------------//
    void            VLAbstractRequestHandler_HandleRequest              ( HTTPServerRequest& request, HTTPServerResponse& response ) const override;
    void            VLAbstractRequestHandler_SetMatch                   ( const PathMatch& match ) override;

protected:

    //----------------------------------------------------------------//
    virtual void    VLAbstractRequestHandlerWithMatch_HandleRequest     ( const PathMatch& match, HTTPServerRequest &request, HTTPServerResponse &response ) const = 0;

public:

    //----------------------------------------------------------------//
                    VLAbstractRequestHandlerWithMatch                   ();
                    ~VLAbstractRequestHandlerWithMatch                  ();
};

#endif
