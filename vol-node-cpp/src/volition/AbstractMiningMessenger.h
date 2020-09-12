// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTMININGMESSENGER_H
#define VOLITION_ABSTRACTMININGMESSENGER_H

#include <volition/common.h>
#include <volition/AbstractMiningMessengerClient.h>

namespace Volition {

//================================================================//
// AbstractMiningMessenger
//================================================================//
class AbstractMiningMessenger {
protected:
    
    //----------------------------------------------------------------//
    virtual void        AbstractMiningMessenger_requestBlock        ( AbstractMiningMessengerClient& client, string minerID, string url, size_t height ) = 0;

public:

    //----------------------------------------------------------------//
    AbstractMiningMessenger () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractMiningMessenger () {
    }

    //----------------------------------------------------------------//
    void requestBlock ( AbstractMiningMessengerClient& client, string minerID, string url, size_t height ) {
    
        this->AbstractMiningMessenger_requestBlock ( client, minerID, url, height );
    }
};

} // namespace Volition
#endif
