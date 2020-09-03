// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPIFACTORY_H
#define VOLITION_WEBMINERAPIFACTORY_H

#include <volition/AbstractAPIRequestHandler.h>
#include <volition/RouteTable.h>
#include <volition/WebMiner.h>

namespace Volition {

class WebMiner;
class WebMinerAPIRequestHandler;

//================================================================//
// WebMinerAPIRequestHandler
//================================================================//
class WebMinerAPIRequestHandler :
    public AbstractAPIRequestHandler {
protected:

    friend class WebMinerAPIFactory;
    
    shared_ptr < WebMiner >     mWebMiner;
};

//================================================================//
// WebMinerAPIFactory
//================================================================//
class WebMinerAPIFactory :
    public Poco::Net::HTTPRequestHandlerFactory {
private:

    RouteTable < WebMinerAPIRequestHandler >    mRouteTable;
    shared_ptr < WebMiner >                     mWebMiner;

    //----------------------------------------------------------------//
    Poco::Net::HTTPRequestHandler*      createRequestHandler        ( const Poco::Net::HTTPServerRequest& request ) override;

public:

    //----------------------------------------------------------------//
                    WebMinerAPIFactory          ( shared_ptr < WebMiner > webMiner );
                    ~WebMinerAPIFactory         ();
};

} // namespace Volition
#endif
