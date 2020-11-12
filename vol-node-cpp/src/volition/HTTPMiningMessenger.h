// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_HTTPMININGMESSENGER_H
#define VOLITION_HTTPMININGMESSENGER_H

#include <volition/common.h>
#include <volition/AbstractMiningMessengerClient.h>

namespace Volition {

//================================================================//
// HTTPMiningMessenger
//================================================================//
class HTTPMiningMessenger :
    public virtual AbstractMiningMessenger {
protected:
    
    Poco::Mutex                         mMutex;
    
    list < MiningMessengerRequest >     mQueue;
    Poco::TaskManager                   mTaskManager;
    Poco::ThreadPool                    mThreadPool;

    //----------------------------------------------------------------//
    void        dispatch                                    ();
    void        onTaskCancelledNotification                 ( Poco::TaskCancelledNotification* pNf );
    void        onTaskFailedNotification                    ( Poco::TaskFailedNotification* pNf );
    void        onTaskFinishedNotification                  ( Poco::TaskFinishedNotification* pNf );

    //----------------------------------------------------------------//
    bool        AbstractMiningMessenger_isBlocked           () const override;
    void        AbstractMiningMessenger_request             ( const MiningMessengerRequest& request ) override;

public:

    //----------------------------------------------------------------//
                HTTPMiningMessenger         ();
                ~HTTPMiningMessenger        ();

};

} // namespace Volition
#endif
