// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_HTTPREQUESTHANDLERFACTORY_H
#define VOLITION_HTTPREQUESTHANDLERFACTORY_H

#include <volition/RouteTable.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// HTTPRequestHandlerFactory
//================================================================//
class HTTPRequestHandlerFactory :
    public Poco::Net::HTTPRequestHandlerFactory {
private:

    RouteTable  mRouteTable;

    //----------------------------------------------------------------//
    Poco::Net::HTTPRequestHandler*      createRequestHandler        ( const Poco::Net::HTTPServerRequest& request ) override;

public:

    //----------------------------------------------------------------//
                    HTTPRequestHandlerFactory               ();
                    ~HTTPRequestHandlerFactory              ();
};

} // WebMinerAPI
} // namespace Volition
#endif
