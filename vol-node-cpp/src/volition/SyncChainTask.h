// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SYNCCHAINTASK_H
#define VOLITION_SYNCCHAINTASK_H

#include <volition/common.h>
#include <volition/WebMiner.h>

namespace Volition {

//================================================================//
// SyncChainTask
//================================================================//
class SyncChainTask :
    public Poco::Task {
private:

    friend class WebMiner;

    string                          mMinerID;
    string                          mURL;
    unique_ptr < BlockQueueEntry >  mBlockQueueEntry;

    //----------------------------------------------------------------//
    void                    runTask                 () override;

public:

    //----------------------------------------------------------------//
                            SyncChainTask           ( string minerID, string url );
                            ~SyncChainTask          ();
};

} // namespace Volition
#endif
