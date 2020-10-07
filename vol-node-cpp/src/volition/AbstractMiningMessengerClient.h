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
    virtual void        AbstractMiningMessengerClient_receiveBlock      ( const MiningMessengerRequest& request, shared_ptr < const Block > block ) = 0;
    virtual void        AbstractMiningMessengerClient_receiveHeaders    ( const MiningMessengerRequest& request, const list < shared_ptr < const BlockHeader >>& headers ) = 0;

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
    void receiveHeaders ( const MiningMessengerRequest& request, const list < shared_ptr < const BlockHeader >>& headers ) {
    
        this->AbstractMiningMessengerClient_receiveHeaders ( request, headers );
    }
};

} // namespace Volition
#endif
