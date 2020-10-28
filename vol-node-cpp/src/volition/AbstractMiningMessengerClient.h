// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTMININGMESSENGERCLIENT_H
#define VOLITION_ABSTRACTMININGMESSENGERCLIENT_H

#include <volition/common.h>
#include <volition/AbstractMiningMessenger.h>

namespace Volition {

class Block;
class BlockHeader;
class MiningMessengerRequest;

//================================================================//
// MiningMessengerResponse
//================================================================//
class MiningMessengerResponse {
public:

    enum Type {
        RESPONSE_BLOCK,
        RESPONSE_ERROR,
        RESPONSE_HEADER,
        RESPONSE_MINER,
        RESPONSE_URL,
    };

    MiningMessengerRequest              mRequest;
    Type                                mType;

    string                              mMinerID;
    shared_ptr < const Block >          mBlock;
    shared_ptr < const BlockHeader >    mHeader;
    string                              mURL;
};

//================================================================//
// AbstractMiningMessengerClient
//================================================================//
class AbstractMiningMessengerClient {
protected:

    //----------------------------------------------------------------//
    virtual void AbstractMiningMessengerClient_receiveBlock ( const MiningMessengerRequest& request, shared_ptr < const Block > block ) {
    
        MiningMessengerResponse response;
        response.mRequest = request;
        response.mType      = MiningMessengerResponse::RESPONSE_BLOCK;
        response.mBlock     = block;
        this->AbstractMiningMessengerClient_receiveResponse ( response );
    }
    
    //----------------------------------------------------------------//
    virtual void AbstractMiningMessengerClient_receiveError ( const MiningMessengerRequest& request ) {
    
        MiningMessengerResponse response;
        response.mType      = MiningMessengerResponse::RESPONSE_ERROR;
        response.mRequest   = request;
        this->AbstractMiningMessengerClient_receiveResponse ( response );
    }
    
    //----------------------------------------------------------------//
    virtual void AbstractMiningMessengerClient_receiveHeader ( const MiningMessengerRequest& request, shared_ptr < const BlockHeader > header ) {
    
        MiningMessengerResponse response;
        response.mType      = MiningMessengerResponse::RESPONSE_HEADER;
        response.mRequest   = request;
        response.mHeader    = header;
        this->AbstractMiningMessengerClient_receiveResponse ( response );
    }
    
    //----------------------------------------------------------------//
    virtual void AbstractMiningMessengerClient_receiveMiner ( const MiningMessengerRequest& request, string minerID, string url ) {
    
        MiningMessengerResponse response;
        response.mType      = MiningMessengerResponse::RESPONSE_MINER;
        response.mRequest   = request;
        response.mMinerID   = minerID;
        response.mURL       = url;
        this->AbstractMiningMessengerClient_receiveResponse ( response );
    }
    
    //----------------------------------------------------------------//
    virtual void AbstractMiningMessengerClient_receiveMinerURL ( const MiningMessengerRequest& request, string url ) {
    
        MiningMessengerResponse response;
        response.mType      = MiningMessengerResponse::RESPONSE_URL;
        response.mRequest   = request;
        response.mURL       = url;
        this->AbstractMiningMessengerClient_receiveResponse ( response );
    }
    
    //----------------------------------------------------------------//
    virtual void        AbstractMiningMessengerClient_receiveResponse       ( const MiningMessengerResponse& response ) = 0;

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
