// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTMININGMESSENGER_H
#define VOLITION_ABSTRACTMININGMESSENGER_H

#include <volition/common.h>
#include <volition/AbstractMiningMessengerClient.h>
#include <volition/Digest.h>

namespace Volition {

class MiningMessengerRequest;

//================================================================//
// MiningMessengerRequest
//================================================================//
class MiningMessengerRequest {
public:

    enum Type {
        UNKNOWN,
        REQUEST_BLOCK,
        REQUEST_HEADER,
    };

    AbstractMiningMessengerClient*      mClient;
    string                              mMinerID;
    string                              mBaseURL;
    Digest                              mBlockDigest;
    size_t                              mHeight;
    Type                                mRequestType;
    
    //----------------------------------------------------------------//
    MiningMessengerRequest () :
        mClient ( NULL ),
        mHeight ( 0 ),
        mRequestType ( UNKNOWN ) {
    }
    
    //----------------------------------------------------------------//
    MiningMessengerRequest ( AbstractMiningMessengerClient& client, string minerID, string baseURL, MiningMessengerRequest::Type requestType ) :
        mClient ( &client ),
        mMinerID ( minerID ),
        mBaseURL ( baseURL ),
        mRequestType ( requestType ) {
    }
};

//================================================================//
// AbstractMiningMessenger
//================================================================//
class AbstractMiningMessenger {
protected:
    
    //----------------------------------------------------------------//
    virtual void        AbstractMiningMessenger_request             ( const MiningMessengerRequest& request ) = 0;

public:

    //----------------------------------------------------------------//
    AbstractMiningMessenger () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractMiningMessenger () {
    }

    //----------------------------------------------------------------//
    void requestBlock ( AbstractMiningMessengerClient& client, string minerID, string baseURL, const Digest& digest ) {
    
        MiningMessengerRequest request ( client, minerID, baseURL, MiningMessengerRequest::REQUEST_BLOCK );
        request.mBlockDigest = digest;
    
        this->AbstractMiningMessenger_request ( request );
    }

    //----------------------------------------------------------------//
    void requestHeader ( AbstractMiningMessengerClient& client, string minerID, string baseURL, size_t height ) {
    
        MiningMessengerRequest request ( client, minerID, baseURL, MiningMessengerRequest::REQUEST_HEADER );
        request.mHeight = height;
    
        this->AbstractMiningMessenger_request ( request );
    }
};

} // namespace Volition
#endif
