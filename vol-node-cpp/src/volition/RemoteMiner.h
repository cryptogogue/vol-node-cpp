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

    //----------------------------------------------------------------//
    void            processHeaders          ( Miner& miner, time_t now );


public:

    enum MinerState {
        STATE_OFFLINE,              // offline for any reason; will retry
        STATE_WAITING_FOR_INFO,     // waiting for miner ID
        STATE_ONLINE,               // active and up to date with a valid branch
        STATE_WAITING_FOR_HEADERS,  // waiting for miner ID
        STATE_ERROR,                // unrecoverable error
    };

    string                      mURL;
    BlockTreeTag                mTag;
    BlockTreeTag                mImproved;
    MinerState                  mState;
    string                      mMessage;

    list < shared_ptr < const BlockHeader >>    mHeaderList;

    GET ( string,       MinerID,        mMinerID )
    GET ( string,       URL,            mURL )

    //----------------------------------------------------------------//
    bool            canFetchInfo            () const;
    bool            canFetchHeaders         () const;
    bool            isContributor           () const;
    bool            isOnline                () const;
    void            receiveResponse         ( Miner& miner, const MiningMessengerResponse& response, time_t now );
                    RemoteMiner             ();
                    ~RemoteMiner            ();
    void            report                  () const;
    void            reset                   ();
    void            setError                ( string message = "" );
    void            setMinerID              ( string minerID );
    void            update                  ( AbstractMiningMessenger& messenger );
};

} // namespace Volition
#endif
