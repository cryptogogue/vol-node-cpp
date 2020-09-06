// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINER_H
#define VOLITION_WEBMINER_H

#include <volition/common.h>
#include <volition/Block.h>
#include <volition/BlockTree.h>
#include <volition/Chain.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>
#include <volition/Transaction.h>

namespace Volition {

class WebMinerHTTPRequestHandler;

//================================================================//
// WebMiner
//================================================================//
class WebMiner :
    public Miner,
    public Poco::Activity < WebMiner > {
private:

    friend class WebMinerHTTPRequestHandlerFactory;

    SerializableTime    mStartTime;
    u32                 mUpdateIntervalInSeconds;
    Poco::Event         mShutdownEvent;

    //----------------------------------------------------------------//
    void                runActivity                 ();

    //----------------------------------------------------------------//
    void                Miner_reset                 () override;
    void                Miner_shutdown              ( bool kill ) override;

public:

    static const u32    DEFAULT_UPDATE_INTERVAL = 60;

    //----------------------------------------------------------------//
    SerializableTime    getStartTime                ();
    void                setSolo                     ( bool solo );
    void                setUpdateInterval           ( u32 updateIntervalInSeconds );
    void                waitForShutdown             ();
                        WebMiner                    ();
                        ~WebMiner                   ();
};

} // namespace Volition
#endif
