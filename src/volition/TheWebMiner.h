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
    public Singleton < TheWebMiner >,
    public Poco::Net::HTTPRequestHandlerFactory {
private:

    friend class ScopedWebMinerLock;

    RouteTable  mRouteTable;
    WebMiner    mWebMiner;

    //----------------------------------------------------------------//
    Poco::Net::HTTPRequestHandler*      createRequestHandler        ( const Poco::Net::HTTPServerRequest& request ) override;

public:

    //----------------------------------------------------------------//
                    TheWebMiner             ();
};

//================================================================//
// ScopedWebMinerLock
//================================================================//
class ScopedWebMinerLock {
private:

    Poco::ScopedLock < Poco::Mutex >    mScopedLock;
    WebMiner&                           mWebMiner;

public:

    //----------------------------------------------------------------//
    WebMiner&       getWebMiner             ();
                    ScopedWebMinerLock      ( TheWebMiner& theWebMiner );
};

} // namespace Volition
#endif
