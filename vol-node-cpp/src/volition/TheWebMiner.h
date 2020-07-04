// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_THEWEBMINER_H
#define VOLITION_THEWEBMINER_H

#include <volition/RouteTable.h>
#include <volition/WebMiner.h>

namespace Volition {

//================================================================//
// TheWebMiner
//================================================================//
class TheWebMiner :
    public Singleton < TheWebMiner > {
private:

    friend class ScopedWebMinerLock;

    WebMiner        mWebMiner;

public:

    //----------------------------------------------------------------//
    void            shutdown                ();
                    TheWebMiner             ();
                    ~TheWebMiner            ();
    void            waitForShutdown         ();
};

//================================================================//
// ScopedWebMinerLock
//================================================================//
class ScopedWebMinerLock {
private:

    Poco::ScopedLock < Poco::Mutex >    mScopedLock;
    WebMiner&                           mWebMiner;

public:

    //----------------------------------------------------------------//
    WebMiner&       getWebMiner             ();
                    ScopedWebMinerLock      ( TheWebMiner& theWebMiner );
};

} // namespace Volition
#endif
