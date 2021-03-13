// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTMINERAPIREQUESTHANDLER_H
#define VOLITION_ABSTRACTMINERAPIREQUESTHANDLER_H

#include <volition/AbstractAPIRequestHandler.h>
#include <volition/HTTP.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>

namespace Volition {

class AbstractConsensusInspector;
class Ledger;
class Miner;

//================================================================//
// AbstractMinerAPIRequestHandler
//================================================================//
class AbstractMinerAPIRequestHandler :
    public AbstractAPIRequestHandler {
protected:

    friend class MinerAPIFactory;
        
    //----------------------------------------------------------------//
    virtual void        AbstractMinerAPIRequestHandler_initialize           ( shared_ptr < Miner > miner ) = 0;

public:

    //----------------------------------------------------------------//
    AbstractMinerAPIRequestHandler () {
    }
    
     //----------------------------------------------------------------//
    ~AbstractMinerAPIRequestHandler () {
    }
    
     //----------------------------------------------------------------//
    void initialize ( shared_ptr < Miner > miner ) {
    
        this->AbstractMinerAPIRequestHandler_initialize ( miner );
    }
};

} // namespace Volition
#endif
