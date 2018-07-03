// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_THEMINER_H
#define VOLITION_THEMINER_H

#include "common.h"
#include "AbstractTransaction.h"
#include "Chain.h"
#include "Singleton.h"
#include "State.h"

namespace Volition {

class AbstractHashable;
class Signable;
class SyncChainTask;

//================================================================//
// TheMiner
//================================================================//
class TheMiner :
    public Singleton < TheMiner >,
    public Poco::Activity < TheMiner >  {
private:

    string                                          mMinerID;

    unique_ptr < Poco::Crypto::ECKey >              mKeyPair;
    list < unique_ptr < AbstractTransaction >>      mPendingTransactions;

    Poco::TaskManager                               mTaskManager;

    unique_ptr < Chain >                            mChain;
    State                                           mState;
    
    map < string, string >                          mMinerURLs;

    //----------------------------------------------------------------//
    void            onSyncChainNotification     ( Poco::TaskFinishedNotification* pNf );
    void            onSyncChainNotification     ( SyncChainTask& task );
    void            pushBlock                   ( Chain& chain, bool force );
    void            run                         () override;

public:

    //----------------------------------------------------------------//
    void            loadGenesis             ( string genesis );
    void            loadKey                 ( string keyfile, string password = "" );
    const Chain*    getChain                () const;
    string          getPublicKey            ();
    void            pushTransaction         ( unique_ptr < AbstractTransaction >& transaction );
    void            setMinerID              ( string minerID );
    void            shutdown                ();
                    TheMiner                ();
                    ~TheMiner               ();
};

} // namespace Volition
#endif
