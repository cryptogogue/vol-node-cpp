// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SYNCCHAINTASK_H
#define VOLITION_SYNCCHAINTASK_H

#include <volition/common.h>
#include <volition/Chain.h>
#include <volition/MinerInfo.h>
#include <volition/Singleton.h>

namespace Volition {

//================================================================//
// SyncChainTask
//================================================================//
class SyncChainTask :
    public Poco::Task {
private:

    string          mMinerID;
    string          mURL;

    bool            mIsValid;
    Chain           mChain;

    //----------------------------------------------------------------//
    void            runTask             () override;

public:

    //----------------------------------------------------------------//
    const Chain&    getChain            () const;
    string          getMinerID          () const;
    string          getURL              () const;
    bool            isValid             () const;
                    SyncChainTask       ( string minerID, string url );
                    ~SyncChainTask      ();
};

} // namespace Volition
#endif
