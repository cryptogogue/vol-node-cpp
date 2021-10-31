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

class Miner;

//================================================================//
// RemoteMiner
//================================================================//
class RemoteMiner :
    public enable_shared_from_this < RemoteMiner > {
private:

    Miner&                      mMiner;
    string                      mMinerID;

    //----------------------------------------------------------------//
    void            processHeaders          ( const MiningMessengerResponse& response, time_t now, u64 acceptedRelease );

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
    MinerState                  mState;
    
    size_t                      mRewind;
    size_t                      mHeight;
    u64                         mAcceptedRelease;
    u64                         mNextRelease;

    GET ( string,       MinerID,        mMinerID )
    GET ( string,       URL,            mURL )

    //----------------------------------------------------------------//
    bool            canFetchInfo            () const;
    bool            canFetchHeaders         () const;
    bool            isContributor           () const;
    bool            isOnline                () const;
    bool            receiveResponse         ( const MiningMessengerResponse& response, time_t now, u64 acceptedRelease );
                    RemoteMiner             ( Miner& miner );
                    ~RemoteMiner            ();
    void            report                  ( u64 minHeight, u64 maxHeight ) const;
    void            reset                   ();
    void            setError                ();
    void            setMinerID              ( string minerID );
    void            update                  ( u64 acceptedRelease );
};

} // namespace Volition
#endif
