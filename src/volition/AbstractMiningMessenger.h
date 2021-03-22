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
        REQUEST_EXTEND_NETWORK,
        REQUEST_HEADERS,
        REQUEST_MINER_INFO,
    };

    Type                                mRequestType;

    string                              mMinerURL;
    Digest                              mBlockDigest;
    size_t                              mHeight;
    
    string                              mDebug;
    
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

    enum Status {
        STATUS_OK,
        STATUS_ERROR,
    };

    MiningMessengerRequest                      mRequest;
    Status                                      mStatus;

    string                                      mMinerID;
    string                                      mGenesisHash;
    shared_ptr < const Block >                  mBlock;
    list < shared_ptr < const BlockHeader >>    mHeaders;
    set < string >                              mMinerURLs;
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
    void enqueueBlockRequest ( string minerURL, const Digest& digest, u64 height, string debug = "" ) {
        
        MiningMessengerRequest request ( minerURL, MiningMessengerRequest::REQUEST_BLOCK );
        request.mBlockDigest    = digest;
        request.mHeight         = height;
        request.mDebug          = debug;
        this->enqueueRequest ( request );
    }

    //----------------------------------------------------------------//
    void enqueueBlockResponse ( const MiningMessengerRequest& request, shared_ptr < const Block > block ) {
    
        MiningMessengerResponse response;
        response.mStatus        = MiningMessengerResponse::STATUS_OK;
        response.mRequest       = request;
        response.mBlock         = block;
        this->enqueueResponse ( response );
    }

    //----------------------------------------------------------------//
    void enqueueErrorResponse ( const MiningMessengerRequest& request ) {
    
        MiningMessengerResponse response;
        response.mStatus        = MiningMessengerResponse::STATUS_ERROR;
        response.mRequest       = request;
        this->enqueueResponse ( response );
    }

    //----------------------------------------------------------------//
    void enqueueExtendNetworkRequest ( string minerURL ) {
        
        MiningMessengerRequest request ( minerURL, MiningMessengerRequest::REQUEST_EXTEND_NETWORK );
        this->enqueueRequest ( request );
    }
    
    //----------------------------------------------------------------//
    void enqueueExtendNetworkResponse ( const MiningMessengerRequest& request, const set < string > urls ) {
    
        MiningMessengerResponse response;
        response.mStatus        = MiningMessengerResponse::STATUS_OK;
        response.mRequest       = request;
        response.mMinerURLs     = urls;
        this->enqueueResponse ( response );
    }

    //----------------------------------------------------------------//
    void enqueueHeadersRequest ( string minerURL, size_t height ) {
        
        MiningMessengerRequest request ( minerURL, MiningMessengerRequest::REQUEST_HEADERS );
        request.mHeight         = height;
        this->enqueueRequest ( request );
    }

    //----------------------------------------------------------------//
    void enqueueHeadersResponse ( const MiningMessengerRequest& request, const list < shared_ptr < const BlockHeader >>& headers ) {
    
        MiningMessengerResponse response;
        response.mStatus        = MiningMessengerResponse::STATUS_OK;
        response.mRequest       = request;
        response.mHeaders       = headers;
        this->enqueueResponse ( response );
    }
    
    //----------------------------------------------------------------//
    void enqueueMinerInfoRequest ( string minerURL ) {
        
        MiningMessengerRequest request ( minerURL, MiningMessengerRequest::REQUEST_MINER_INFO );
        this->enqueueRequest ( request );
    }
    
    //----------------------------------------------------------------//
    void enqueueMinerInfoResponse ( const MiningMessengerRequest& request, string minerID, string genesisHash ) {
    
        MiningMessengerResponse response;
        response.mStatus        = MiningMessengerResponse::STATUS_OK;
        response.mRequest       = request;
        response.mMinerID       = minerID;
        response.mGenesisHash   = genesisHash;
        this->enqueueResponse ( response );
    }

    //----------------------------------------------------------------//
    void sendRequests () {
    
        LGN_LOG_SCOPE ( VOL_FILTER_CONSENSUS, INFO, __PRETTY_FUNCTION__ );
    
        list < MiningMessengerRequest > requests;
        {
            Poco::ScopedLock < Poco::Mutex > lock ( this->mRequestMutex );
            requests = this->mRequestQueue;
            this->mRequestQueue.clear ();
        }
                
        for ( ; requests.size (); requests.pop_front ()) {
            const MiningMessengerRequest& request = requests.front ();
            this->AbstractMiningMessenger_sendRequest ( request );
        }
    }
    
    //----------------------------------------------------------------//
    void receiveResponses ( AbstractMiningMessengerClient& client, time_t now ) {
    
        LGN_LOG_SCOPE ( VOL_FILTER_CONSENSUS, INFO, __PRETTY_FUNCTION__ );
    
        list < MiningMessengerResponse > responses;
        {
            Poco::ScopedLock < Poco::Mutex > lock ( this->mResponseMutex );
            responses = this->mResponseQueue;
            this->mResponseQueue.clear ();
        }
        
        for ( ; responses.size (); responses.pop_front ()) {
            const MiningMessengerResponse& response = responses.front ();
            client.receiveResponse ( response, now );
        }
    }
};

} // namespace Volition
#endif
