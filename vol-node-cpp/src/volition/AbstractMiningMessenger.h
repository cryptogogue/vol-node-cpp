// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTMININGMESSENGER_H
#define VOLITION_ABSTRACTMININGMESSENGER_H

#include <volition/common.h>
#include <volition/Digest.h>

namespace Volition {

class AbstractMiningMessengerClient;

//================================================================//
// MiningMessengerRequest
//================================================================//
class MiningMessengerRequest {
public:

    enum Type {
        UNKNOWN,
        REQUEST_BLOCK,
        REQUEST_HEADERS,
        REQUEST_MINER,
        REQUEST_MINER_URLS,
        REQUEST_PREV_HEADERS,
    };

    Type                                mRequestType;

    AbstractMiningMessengerClient*      mClient;
    string                              mMinerURL;
    Digest                              mBlockDigest;
    size_t                              mHeight;
    
    //----------------------------------------------------------------//
    MiningMessengerRequest () :
        mRequestType ( UNKNOWN ),
        mClient ( NULL ),
        mHeight ( 0 ) {
    }
    
    //----------------------------------------------------------------//
    MiningMessengerRequest ( AbstractMiningMessengerClient& client, string minerURL, MiningMessengerRequest::Type requestType ) :
        mClient ( &client ),
        mMinerURL ( minerURL ),
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
    void requestBlock ( AbstractMiningMessengerClient& client, string minerURL, const Digest& digest ) {
        
        MiningMessengerRequest request ( client, minerURL, MiningMessengerRequest::REQUEST_BLOCK );
        request.mBlockDigest = digest;
        
        this->AbstractMiningMessenger_request ( request );
    }

    //----------------------------------------------------------------//
    void requestHeader ( AbstractMiningMessengerClient& client, string minerURL, size_t height, bool forward = true ) {
        
        MiningMessengerRequest request ( client, minerURL, forward ? MiningMessengerRequest::REQUEST_HEADERS : MiningMessengerRequest::REQUEST_PREV_HEADERS );
        request.mHeight = height;
        
        this->AbstractMiningMessenger_request ( request );
    }
    
    //----------------------------------------------------------------//
    void requestMiner ( AbstractMiningMessengerClient& client, string minerURL ) {
        
        MiningMessengerRequest request ( client, minerURL, MiningMessengerRequest::REQUEST_MINER );
        
        this->AbstractMiningMessenger_request ( request );
    }
    
    //----------------------------------------------------------------//
    void requestMinerURLs ( AbstractMiningMessengerClient& client, string minerURL ) {
        
        MiningMessengerRequest request ( client, minerURL, MiningMessengerRequest::REQUEST_MINER_URLS );
        
        this->AbstractMiningMessenger_request ( request );
    }
};

} // namespace Volition
#endif
