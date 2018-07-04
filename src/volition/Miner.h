// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINER_H
#define VOLITION_MINER_H

#include "common.h"
#include "AbstractTransaction.h"
#include "Chain.h"
#include "Singleton.h"
#include "State.h"

namespace Volition {

class AbstractHashable;
class Block;

//================================================================//
// Miner
//================================================================//
class Miner {
protected:

    string                                          mMinerID;

    unique_ptr < Poco::Crypto::ECKey >              mKeyPair;
    list < unique_ptr < AbstractTransaction >>      mPendingTransactions;

    unique_ptr < Chain >                            mChain;
    State                                           mState;
    
    //----------------------------------------------------------------//
    void            pushBlock               ( Chain& chain, bool force );

public:

    //----------------------------------------------------------------//
    void            loadGenesis             ( string path );
    void            loadKey                 ( string keyfile, string password = "" );
    const Chain*    getChain                () const;
    //string          getPublicKey            ();
    void            pushTransaction         ( unique_ptr < AbstractTransaction >& transaction );
    void            setGenesis              ( shared_ptr < Block > block );
    void            setMinerID              ( string minerID );
                    Miner                   ();
    virtual         ~Miner                  ();
    void            updateChain             ( unique_ptr < Chain > proposedChain );
};

} // namespace Volition
#endif
