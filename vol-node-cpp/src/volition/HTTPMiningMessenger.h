// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_HTTPMININGMESSENGER_H
#define VOLITION_HTTPMININGMESSENGER_H

#include <volition/common.h>
#include <volition/AbstractMiningMessenger.h>

namespace Volition {

//================================================================//
// HTTPMiningMessengerRequestQueue
//================================================================//
class HTTPMiningMessengerRequestQueue {
protected:
    
    friend class HTTPMiningMessenger;
    
    size_t                              mActiveCount;
    size_t                              mRawWeight;
    double                              mWeight;
    
    list < MiningMessengerRequest >     mPending;

public:

    //----------------------------------------------------------------//
                    HTTPMiningMessengerRequestQueue             ();
                    ~HTTPMiningMessengerRequestQueue            ();
    void            pushRequest                                 ( const MiningMessengerRequest& request );
};

//================================================================//
// HTTPMiningMessenger
//================================================================//
class HTTPMiningMessenger :
    public virtual AbstractMiningMessenger {
protected:
    
    enum {
        BLOCK_QUEUE_INDEX               = 0,
        HEADER_QUEUE_INDEX,
        INFO_QUEUE_INDEX,
        TOPOLOGY_QUEUE_INDEX,
        TOTAL_QUEUES,
    };
    
    enum {
        BLOCK_QUEUE_WEIGHT              = 4,
        HEADER_QUEUE_WEIGHT             = 8,
        INFO_QUEUE_WEIGHT               = 2,
        TOPOLOGY_QUEUE_WEIGHT           = 2,
    };
    
    Poco::Mutex                         mMutex;
    
    Poco::TaskManager                   mTaskManager;
    Poco::ThreadPool                    mThreadPool;

    size_t                              mTotalPending;
    size_t                              mTotalActive;

    map < size_t, HTTPMiningMessengerRequestQueue > mQueues;

    //----------------------------------------------------------------//
    void                completeRequest                             ( const MiningMessengerRequest& request );
    static void         deserailizeHeaderList                       ( list < shared_ptr < const BlockHeader >>& responseHeaders, Poco::JSON::Array::Ptr headersJSON );
    size_t              getQueueIndex                               ( const MiningMessengerRequest& request ) const;
    size_t              getQueueRawWeight                           ( size_t index ) const;
    string              getRequestURL                               ( const MiningMessengerRequest& request ) const;
    void                onTaskCancelledNotification                 ( Poco::TaskCancelledNotification* pNf );
    void                onTaskFailedNotification                    ( Poco::TaskFailedNotification* pNf );
    void                onTaskFinishedNotification                  ( Poco::TaskFinishedNotification* pNf );
    void                pumpQueues                                  ();
    void                sendRequest                                 ( HTTPMiningMessengerRequestQueue& queue );

    //----------------------------------------------------------------//
    void                AbstractMiningMessenger_await               () override;
    void                AbstractMiningMessenger_sendRequest         ( const MiningMessengerRequest& request ) override;

public:

    //----------------------------------------------------------------//
                        HTTPMiningMessenger                         ();
                        ~HTTPMiningMessenger                        ();
};

} // namespace Volition
#endif
