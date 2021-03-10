// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINERAPIFACTORY_H
#define VOLITION_MINERAPIFACTORY_H

#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/RouteTable.h>
#include <volition/Miner.h>

namespace Volition {

//================================================================//
// MinerAPIFactory
//================================================================//
class MinerAPIFactory :
    public Poco::Net::HTTPRequestHandlerFactory {
private:

    RouteTable < AbstractAPIRequestHandlerWithMiner >   mRouteTable;
    shared_ptr < Miner >                                mMiner;
    map < string, shared_ptr < Miner >>                 mMiners;
    bool                                                mWithPrefix;

    //----------------------------------------------------------------//
    Poco::Net::HTTPRequestHandler*      createRequestHandler        ( const Poco::Net::HTTPServerRequest& request ) override;
    void                                initializeRoutes            ();

public:

    //----------------------------------------------------------------//
                    MinerAPIFactory          ( shared_ptr < Miner > minerActivity );
                    MinerAPIFactory          ( const vector < shared_ptr < Miner >>& webMiners );
                    ~MinerAPIFactory         ();
};

} // namespace Volition
#endif
