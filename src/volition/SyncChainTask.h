// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SYNCCHAINTASK_H
#define VOLITION_SYNCCHAINTASK_H

#include <volition/common.h>
#include <volition/MinerInfo.h>
#include <volition/Singleton.h>

namespace Volition {

class Chain;

//================================================================//
// SyncChainTask
//================================================================//
class SyncChainTask :
    public Poco::Task {
private:

    string      mMinerID;
    string      mURL;

    unique_ptr < Chain >    mChain;

    //----------------------------------------------------------------//
    void                    runTask             () override;

public:

    //----------------------------------------------------------------//
    string                  getMinerID          () const;
    string                  getURL              () const;
    unique_ptr < Chain >    moveChain           ();
                            SyncChainTask       ( string minerID, string url );
                            ~SyncChainTask      ();
};

} // namespace Volition
#endif
