// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINERAPIFACTORY_H
#define VOLITION_MINERAPIFACTORY_H

#include <volition/AbstractAPIRequestHandler.h>
#include <volition/RouteTable.h>
#include <volition/Miner.h>

namespace Volition {

//================================================================//
// MinerAPIRequestHandler
//================================================================//
class MinerAPIRequestHandler :
    public AbstractAPIRequestHandler {
protected:

    friend class MinerAPIFactory;
    
    shared_ptr < Miner >     mWebMiner;
};

//================================================================//
// MinerAPIFactory
//================================================================//
class MinerAPIFactory :
    public Poco::Net::HTTPRequestHandlerFactory {
private:

    RouteTable < MinerAPIRequestHandler >   mRouteTable;
    shared_ptr < Miner >                    mMiner;
    map < string, shared_ptr < Miner >>     mMiners;
    bool                                    mWithPrefix;

    //----------------------------------------------------------------//
    Poco::Net::HTTPRequestHandler*      createRequestHandler        ( const Poco::Net::HTTPServerRequest& request ) override;
    void                                initializeRoutes            ();

public:

    //----------------------------------------------------------------//
                    MinerAPIFactory          ( shared_ptr < Miner > webMiner );
                    MinerAPIFactory          ( const vector < shared_ptr < Miner >>& webMiners );
                    ~MinerAPIFactory         ();
};

} // namespace Volition
#endif
