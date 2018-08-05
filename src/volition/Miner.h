// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINER_H
#define VOLITION_MINER_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/CryptoKey.h>
#include <volition/Chain.h>
#include <volition/Singleton.h>
#include <volition/State.h>

namespace Volition {

class AbstractHashable;
class Block;

//================================================================//
// Miner
//================================================================//
class Miner {
protected:

    string                                          mMinerID;

    CryptoKey                                       mKeyPair;
    list < shared_ptr < AbstractTransaction >>      mPendingTransactions;
    bool                                            mLazy;

    unique_ptr < Chain >                            mChain;
    
    //----------------------------------------------------------------//
    unique_ptr < Block >    makeBlock               ( Chain& chain );
    void                    pushBlock               ( Chain& chain, bool force );

public:

    //----------------------------------------------------------------//
    void                    loadGenesis             ( string path );
    void                    loadKey                 ( string keyfile, string password = "" );
    Chain*                  getChain                () const;
    string                  getMinerID              () const;
    const State&            getState                () const;
    void                    pushTransaction         ( shared_ptr < AbstractTransaction > transaction );
    void                    setGenesis              ( shared_ptr < Block > block );
    void                    setLazy                 ( bool lazy );
    void                    setMinerID              ( string minerID );
                            Miner                   ();
    virtual                 ~Miner                  ();
    void                    updateChain             ( unique_ptr < Chain > proposedChain );
};

} // namespace Volition
#endif
