// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINER_H
#define VOLITION_MINER_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/CryptoKey.h>
#include <volition/Chain.h>
#include <volition/ChainMetadata.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/Singleton.h>
#include <volition/Ledger.h>

namespace Volition {

class AbstractHashable;
class Block;

//================================================================//
// Miner
//================================================================//
class Miner :
    public AbstractSerializable {
protected:

    string                                          mMinerID;

    CryptoKey                                       mKeyPair;
    list < shared_ptr < AbstractTransaction >>      mPendingTransactions;
    bool                                            mLazy;

    string                                          mChainPath;

    Chain                                           mChain;
    ChainMetadata                                   mMetadata;
        
    //----------------------------------------------------------------//
    void                    addTransactions         ( Chain& chain, Block& block );
    Digest                  computeAllure           ( size_t cycleID ) const;
    size_t                  computeScore            ( const Digest& allure ) const;
    void                    pushBlock               ( Chain& chain, bool force );
    void                    saveChain               ();

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
    void                    extendChain             ();
    void                    loadGenesis             ( string path );
    void                    loadKey                 ( string keyfile, string password = "" );
    const Chain&            getChain                () const;
    size_t                  getChainSize            () const;
    bool                    getLazy                 () const;
    const Ledger&           getLedger               () const;
    string                  getMinerID              () const;
    void                    pushTransaction         ( shared_ptr < AbstractTransaction > transaction );
    void                    setChainPath            ( string path );
    void                    setGenesis              ( const Block& block );
    void                    setLazy                 ( bool lazy );
    void                    setMinerID              ( string minerID );
                            Miner                   ();
    virtual                 ~Miner                  ();
    void                    updateChain             ( const Chain& proposedChain );
};

} // namespace Volition
#endif
