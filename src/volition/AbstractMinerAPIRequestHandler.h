// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTMINERAPIHANDLER_H
#define VOLITION_ABSTRACTMINERAPIHANDLER_H

#include <volition/AbstractAPIRequestHandler.h>

namespace Volition {

class AbstractConsensusInspector;
class Ledger;
class Miner;

//================================================================//
// AbstractAPIRequestHandlerWithMiner
//================================================================//
class AbstractAPIRequestHandlerWithMiner :
    public AbstractAPIRequestHandler {
protected:

    friend class MinerAPIFactory;
        
    //----------------------------------------------------------------//
    virtual void        AbstractAPIRequestHandlerWithMiner_initialize           ( shared_ptr < Miner > miner ) = 0;

public:

    //----------------------------------------------------------------//
                        AbstractAPIRequestHandlerWithMiner          ();
                        ~AbstractAPIRequestHandlerWithMiner         ();
    void                initialize                                  ( shared_ptr < Miner > miner );
};

//================================================================//
// AbstractMinerAPIRequestHandler
//================================================================//
class AbstractMinerAPIRequestHandler :
    public AbstractAPIRequestHandlerWithMiner {
protected:

    friend class MinerAPIFactory;
    
    shared_ptr < Miner >    mWebMiner;
    
    //----------------------------------------------------------------//
    HTTPStatus              AbstractAPIRequestHandler_handleRequest             ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override;
    void                    AbstractAPIRequestHandlerWithMiner_initialize       ( shared_ptr < Miner > miner ) override;
    virtual HTTPStatus      AbstractMinerAPIRequestHandler_handleRequest        ( HTTP::Method method, Ledger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const = 0;

public:

    //----------------------------------------------------------------//
                            AbstractMinerAPIRequestHandler          ();
                            ~AbstractMinerAPIRequestHandler         ();
};

//================================================================//
// AbstractConsensusInspectorAPIRequestHandler
//================================================================//
class AbstractConsensusInspectorAPIRequestHandler :
    public AbstractAPIRequestHandlerWithMiner {
protected:

    friend class MinerAPIFactory;
    
    shared_ptr < AbstractConsensusInspector >    mInspector;
    
    //----------------------------------------------------------------//
    HTTPStatus              AbstractAPIRequestHandler_handleRequest                         ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override;
    void                    AbstractAPIRequestHandlerWithMiner_initialize                   ( shared_ptr < Miner > miner ) override;
    virtual HTTPStatus      AbstractConsensusInspectorAPIRequestHandler_handleRequest       ( HTTP::Method method, AbstractConsensusInspector& inspector, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const = 0;

public:

    //----------------------------------------------------------------//
                            AbstractConsensusInspectorAPIRequestHandler         ();
                            ~AbstractConsensusInspectorAPIRequestHandler        ();
};

} // namespace Volition
#endif
