// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTMININGMESSENGERCLIENT_H
#define VOLITION_ABSTRACTMININGMESSENGERCLIENT_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// AbstractMiningMessengerClient
//================================================================//
class AbstractMiningMessengerClient {
protected:

    //----------------------------------------------------------------//
    virtual void        AbstractMiningMessengerClient_receiveBlock      ( string minerID, shared_ptr < const Block > block ) = 0;

public:

    //----------------------------------------------------------------//
    AbstractMiningMessengerClient () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractMiningMessengerClient () {
    }

    //----------------------------------------------------------------//
    void receiveBlock ( string minerID, shared_ptr < const Block > block ) {
    
        this->AbstractMiningMessengerClient_receiveBlock ( minerID, block );
    }
};

} // namespace Volition
#endif
