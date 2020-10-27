// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTMININGMESSENGERCLIENT_H
#define VOLITION_ABSTRACTMININGMESSENGERCLIENT_H

#include <volition/common.h>

namespace Volition {

class Block;
class BlockHeader;
class MiningMessengerRequest;

//================================================================//
// AbstractMiningMessengerClient
//================================================================//
class AbstractMiningMessengerClient {
protected:

    //----------------------------------------------------------------//
    virtual void        AbstractMiningMessengerClient_receiveBlock          ( const MiningMessengerRequest& request, shared_ptr < const Block > block ) = 0;
    virtual void        AbstractMiningMessengerClient_receiveError          ( const MiningMessengerRequest& request ) = 0;
    virtual void        AbstractMiningMessengerClient_receiveHeader         ( const MiningMessengerRequest& request, shared_ptr < const BlockHeader > header ) = 0;
    virtual void        AbstractMiningMessengerClient_receiveMiner          ( const MiningMessengerRequest& request, string minerID, string url ) = 0;
    virtual void        AbstractMiningMessengerClient_receiveMinerURL       ( const MiningMessengerRequest& request, string url ) = 0;

public:

    //----------------------------------------------------------------//
    AbstractMiningMessengerClient () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractMiningMessengerClient () {
    }

    //----------------------------------------------------------------//
    void receiveBlock ( const MiningMessengerRequest& request, shared_ptr < const Block > block ) {
    
        this->AbstractMiningMessengerClient_receiveBlock ( request, block );
    }

    //----------------------------------------------------------------//
    void receiveError ( const MiningMessengerRequest& request ) {
    
        this->AbstractMiningMessengerClient_receiveError ( request );
    }

    //----------------------------------------------------------------//
    void receiveHeader ( const MiningMessengerRequest& request, shared_ptr < const BlockHeader > header ) {
    
        this->AbstractMiningMessengerClient_receiveHeader ( request, header );
    }
    
    //----------------------------------------------------------------//
    void receiveMiner ( const MiningMessengerRequest& request, string minerID, string url ) {
    
        this->AbstractMiningMessengerClient_receiveMiner ( request, minerID, url );
    }
    
    //----------------------------------------------------------------//
    void receiveMinerURL ( const MiningMessengerRequest& request, string url ) {
    
        this->AbstractMiningMessengerClient_receiveMinerURL ( request, url );
    }
};

} // namespace Volition
#endif
