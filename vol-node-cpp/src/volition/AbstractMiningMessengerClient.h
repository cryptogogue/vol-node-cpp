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
    virtual void        AbstractMiningMessengerClient_receive           ( const MiningMessengerRequest& request, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block ) = 0;

public:

    //----------------------------------------------------------------//
    AbstractMiningMessengerClient () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractMiningMessengerClient () {
    }

    //----------------------------------------------------------------//
    void receive ( const MiningMessengerRequest& request, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block ) {
    
        this->AbstractMiningMessengerClient_receive ( request, header, block );
    }
};

} // namespace Volition
#endif
