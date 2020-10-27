// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_HTTPMININGMESSENGER_H
#define VOLITION_HTTPMININGMESSENGER_H

#include <volition/common.h>
#include <volition/AbstractMiningMessenger.h>

namespace Volition {

//================================================================//
// HTTPMiningMessenger
//================================================================//
class HTTPMiningMessenger :
    public virtual AbstractMiningMessenger {
protected:
    
    Poco::ThreadPool        mTaskManagerThreadPool;
    Poco::TaskManager       mTaskManager;

    //----------------------------------------------------------------//
    void        onTaskCancelledNotification                 ( Poco::TaskCancelledNotification* pNf );
    void        onTaskFailedNotification                    ( Poco::TaskFailedNotification* pNf );
    void        onTaskFinishedNotification                  ( Poco::TaskFinishedNotification* pNf );

    //----------------------------------------------------------------//
    void        AbstractMiningMessenger_request             ( const MiningMessengerRequest& request ) override;

public:

    //----------------------------------------------------------------//
                HTTPMiningMessenger         ();
                ~HTTPMiningMessenger        ();

};

} // namespace Volition
#endif
