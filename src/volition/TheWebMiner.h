// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_THEWEBMINER_H
#define VOLITION_THEWEBMINER_H

#include <volition/RouteTable.h>
#include <volition/WebMiner.h>

namespace Volition {

//================================================================//
// TheWebMiner
//================================================================//
class TheWebMiner :
    public WebMiner,
    public Singleton < TheWebMiner >,
    public Poco::Net::HTTPRequestHandlerFactory {
private:

    RouteTable mRouteTable;

    //----------------------------------------------------------------//
    Poco::Net::HTTPRequestHandler*      createRequestHandler        ( const Poco::Net::HTTPServerRequest& request ) override;

public:

    //----------------------------------------------------------------//
                                        TheWebMiner                 ();
};

} // namespace Volition
#endif
