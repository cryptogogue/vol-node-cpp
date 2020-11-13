// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTMININGMESSENGER_H
#define VOLITION_ABSTRACTMININGMESSENGER_H

#include <volition/common.h>
#include <volition/Block.h>
#include <volition/Digest.h>

namespace Volition {

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

    string                              mMinerURL;
    Digest                              mBlockDigest;
    size_t                              mHeight;
    
    //----------------------------------------------------------------//
    MiningMessengerRequest () :
        mRequestType ( UNKNOWN ),
        mHeight ( 0 ) {
    }
    
    //----------------------------------------------------------------//
    MiningMessengerRequest ( string minerURL, MiningMessengerRequest::Type requestType ) :
        mRequestType ( requestType ),
        mMinerURL ( minerURL ),
        mHeight ( 0 ) {
    }
};

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
    virtual void        AbstractMiningMessengerClient_receiveResponse       ( const MiningMessengerResponse& response, time_t now ) = 0;

public:

    //----------------------------------------------------------------//
    AbstractMiningMessengerClient () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractMiningMessengerClient () {
    }

    //----------------------------------------------------------------//
    void receiveResponse ( const MiningMessengerResponse& response, time_t now ) {
    
        this->AbstractMiningMessengerClient_receiveResponse ( response, now );
    }
};

//================================================================//
// AbstractMiningMessenger
//================================================================//
class AbstractMiningMessenger {
protected:
        
    Poco::Mutex                         mRequestMutex;
    list < MiningMessengerRequest >     mRequestQueue;
    
    Poco::Mutex                         mResponseMutex;
    list < MiningMessengerResponse >    mResponseQueue;
    
    //----------------------------------------------------------------//
    virtual bool        AbstractMiningMessenger_isBlocked           () const = 0;
    virtual void        AbstractMiningMessenger_sendRequest         ( const MiningMessengerRequest& request ) = 0;

    //----------------------------------------------------------------//
    void enqueueRequest ( const MiningMessengerRequest& request ) {
        Poco::ScopedLock < Poco::Mutex > lock ( this->mRequestMutex );
        this->mRequestQueue.push_back ( request );
    }

    //----------------------------------------------------------------//
    void enqueueResponse ( const MiningMessengerResponse& response ) {
        
        Poco::ScopedLock < Poco::Mutex > lock ( this->mResponseMutex );
        this->mResponseQueue.push_back ( response );
    }

public:

    //----------------------------------------------------------------//
    AbstractMiningMessenger () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractMiningMessenger () {
    }

    //----------------------------------------------------------------//
    void enqueueBlockRequest ( string minerURL, const Digest& digest ) {
        
        MiningMessengerRequest request ( minerURL, MiningMessengerRequest::REQUEST_BLOCK );
        request.mBlockDigest = digest;
        this->enqueueRequest ( request );
    }

    //----------------------------------------------------------------//
    void enqueueBlockResponse ( const MiningMessengerRequest& request, shared_ptr < const Block > block ) {
    
        MiningMessengerResponse response;
        response.mRequest = request;
        response.mType      = MiningMessengerResponse::RESPONSE_BLOCK;
        response.mBlock     = block;
        this->enqueueResponse ( response );
    }

    //----------------------------------------------------------------//
    void enqueueErrorResponse ( const MiningMessengerRequest& request ) {
    
        MiningMessengerResponse response;
        response.mType      = MiningMessengerResponse::RESPONSE_ERROR;
        response.mRequest   = request;
        this->enqueueResponse ( response );
    }

    //----------------------------------------------------------------//
    void enqueueHeaderRequest ( string minerURL, size_t height, bool forward = true ) {
        
        MiningMessengerRequest request ( minerURL, forward ? MiningMessengerRequest::REQUEST_HEADERS : MiningMessengerRequest::REQUEST_PREV_HEADERS );
        request.mHeight = height;
        this->enqueueRequest ( request );
    }

    //----------------------------------------------------------------//
    void enqueueHeaderResponse ( const MiningMessengerRequest& request, shared_ptr < const BlockHeader > header ) {
    
        MiningMessengerResponse response;
        response.mType      = MiningMessengerResponse::RESPONSE_HEADER;
        response.mRequest   = request;
        response.mHeader    = header;
        this->enqueueResponse ( response );
    }
    
    //----------------------------------------------------------------//
    void enqueueMinerRequest ( string minerURL ) {
        
        MiningMessengerRequest request ( minerURL, MiningMessengerRequest::REQUEST_MINER );
        this->enqueueRequest ( request );
    }
    
    //----------------------------------------------------------------//
    void enqueueMinerResponse ( const MiningMessengerRequest& request, string minerID, string url ) {
    
        MiningMessengerResponse response;
        response.mType      = MiningMessengerResponse::RESPONSE_MINER;
        response.mRequest   = request;
        response.mMinerID   = minerID;
        response.mURL       = url;
        this->enqueueResponse ( response );
    }
    
    //----------------------------------------------------------------//
    void enqueueMinerURLsRequest ( string minerURL ) {
        
        MiningMessengerRequest request ( minerURL, MiningMessengerRequest::REQUEST_MINER_URLS );
        this->enqueueRequest ( request );
    }
    
    //----------------------------------------------------------------//
    void enqueueMinerURLResponse ( const MiningMessengerRequest& request, string url ) {
    
        MiningMessengerResponse response;
        response.mType      = MiningMessengerResponse::RESPONSE_URL;
        response.mRequest   = request;
        response.mURL       = url;
        this->enqueueResponse ( response );
    }

    //----------------------------------------------------------------//
    bool isBlocked () const {
    
        return this->AbstractMiningMessenger_isBlocked ();
    }

    //----------------------------------------------------------------//
    void sendRequests () {
    
        list < MiningMessengerRequest > requests;
        {
            Poco::ScopedLock < Poco::Mutex > lock ( this->mRequestMutex );
            requests = this->mRequestQueue;
        }
        
        for ( ; requests.size (); requests.pop_front ()) {
            const MiningMessengerRequest& request = requests.front ();
            this->AbstractMiningMessenger_sendRequest ( request );
        }
    }
    
    //----------------------------------------------------------------//
    void receiveResponses ( AbstractMiningMessengerClient& client, time_t now ) {
    
        list < MiningMessengerResponse > responses;
        {
            Poco::ScopedLock < Poco::Mutex > lock ( this->mResponseMutex );
            responses = this->mResponseQueue;
        }
        
        for ( ; responses.size (); responses.pop_front ()) {
            const MiningMessengerResponse& response = responses.front ();
            client.receiveResponse ( response, now );
        }
    }
};

} // namespace Volition
#endif
