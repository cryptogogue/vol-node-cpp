// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_REMOTEMINER_H
#define VOLITION_REMOTEMINER_H

#include <volition/common.h>
#include <volition/Accessors.h>
#include <volition/BlockTree.h>
#include <volition/CryptoKey.h>
#include <volition/Ledger.h>
#include <volition/TransactionQueue.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/Singleton.h>
#include <volition/TransactionStatus.h>

namespace Volition {

//================================================================//
// RemoteMiner
//================================================================//
class RemoteMiner {
public:

    enum NetworkState {
        STATE_NEW,
        STATE_TIMEOUT,
        STATE_ONLINE,
        STATE_ERROR,
    };

    string                      mMinerID;
    string                      mURL;
    BlockTreeNode::ConstPtr     mTag;
    BlockTreeNode::ConstPtr     mImproved;
    NetworkState                mNetworkState;
    string                      mMessage;

    size_t                                              mHeight;
    bool                                                mForward;
    map < size_t, shared_ptr < const BlockHeader >>     mHeaderQueue;

    //----------------------------------------------------------------//
                    RemoteMiner             ();
                    ~RemoteMiner            ();
    void            reset                   ();
    void            setError                ( string message = "" );
    void            updateHeaders           ( BlockTree& blockTree );
};

} // namespace Volition
#endif
