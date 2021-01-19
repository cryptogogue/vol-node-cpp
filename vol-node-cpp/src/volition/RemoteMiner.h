// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_REMOTEMINER_H
#define VOLITION_REMOTEMINER_H

#include <volition/common.h>
#include <volition/AbstractBlockTree.h>
#include <volition/Accessors.h>
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
class RemoteMiner :
    public enable_shared_from_this < RemoteMiner > {
private:

    string                      mMinerID;

public:

    enum NetworkState {
        STATE_NEW,
        STATE_TIMEOUT,
        STATE_ONLINE,
        STATE_ERROR,
        
//        STATE_OFFLINE,
//        STATE_EXTEND_HEADERS,
//        STATE_REWIND_HEADERS,
//        STATE_RETRY,
    };

    string                      mURL;
    BlockTreeTag                mTag;
    BlockTreeTag                mImproved;
    NetworkState                mNetworkState;
    string                      mMessage;

    bool                        mIsBusy;

    size_t                                              mHeight;
    bool                                                mForward;
    map < size_t, shared_ptr < const BlockHeader >>     mHeaderQueue;

    GET ( string,       MinerID,        mMinerID )
    GET ( string,       URL,            mURL )

    //----------------------------------------------------------------//
    bool            canFetchInfo            () const;
    bool            canFetchHeaders         () const;
    void            receiveResponse         ( Miner& miner, const MiningMessengerResponse& response, time_t now );
                    RemoteMiner             ();
                    ~RemoteMiner            ();
    void            reset                   ();
    void            setError                ( string message = "" );
    void            setMinerID              ( string minerID );
    void            updateHeaders           ( AbstractBlockTree& blockTree );
    void            update                  ( AbstractMiningMessenger& messenger );
};

} // namespace Volition
#endif
