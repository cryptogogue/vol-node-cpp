// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINER_H
#define VOLITION_MINER_H

#include <volition/common.h>
#include <volition/BlockTree.h>
#include <volition/CryptoKey.h>
#include <volition/Chain.h>
#include <volition/ChainMetadata.h>
#include <volition/Ledger.h>
#include <volition/TransactionQueue.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/Singleton.h>
#include <volition/Transaction.h>

namespace Volition {

class AbstractChainRecorder;
class AbstractHashable;
class Block;
class BlockTreeNode;

//================================================================//
// Miner
//================================================================//
class Miner :
    public AbstractSerializable,
    public TransactionQueue {
protected:

    friend class AbstractChainRecorder;

    static constexpr const char* MASTER_BRANCH      = "master";

    string                                          mMinerID;

    CryptoKey                                       mKeyPair;
    bool                                            mLazy;
    bool                                            mControlPermitted;

    Block::VerificationPolicy                       mBlockVerificationPolicy;

    shared_ptr < AbstractChainRecorder >            mChainRecorder;
    
    BlockTree                                       mBlockTree;
    shared_ptr < Chain >                            mChain;
    BlockTreeTag                                    mTag;
    
    //----------------------------------------------------------------//
    static void             checkEnvironment        ();
    void                    pushBlock               ( shared_ptr < const Block > block );
    void                    rebuildChain            ();
    void                    rebuildChainRecurse     ( shared_ptr < const BlockTreeNode > node );
    void                    saveChain               ();

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;
    virtual time_t          Miner_getTime                           () const;
    virtual void            Miner_reset                             ();
    virtual void            Miner_shutdown                          ( bool kill ) = 0;

public:

    enum class SubmissionResponse {
        ACCEPTED = 0,
        RESUBMIT_EARLIER,
    };

    //----------------------------------------------------------------//
    void                    affirmKey               ();
    bool                    checkBestBranch         ( string miners ) const;
    bool                    controlPermitted        () const;
    size_t                  countBranches           () const;
    void                    extend                  ( bool force = false );
    const Chain*            getBestBranch           () const;
    const BlockTree&        getBlockTree            () const;
    const CryptoKey&        getKeyPair              () const;
    size_t                  getLongestBranchSize    () const;
    bool                    getLazy                 () const;
    Ledger&                 getLedger               ();
    const Ledger&           getLedger               () const;
    string                  getMinerID              () const;
    time_t                  getTime                 () const;
    void                    loadGenesis             ( string path );
    void                    loadKey                 ( string keyfile, string password = "" );
                            Miner                   ();
    virtual                 ~Miner                  ();
    void                    permitControl           ( bool permit );
    shared_ptr < Block >    prepareBlock            ();
    void                    setChainRecorder        ( shared_ptr < AbstractChainRecorder > chainRecorder );
    void                    setGenesis              ( shared_ptr < const Block > block );
    void                    setLazy                 ( bool lazy );
    void                    setMinerID              ( string minerID );
    void                    reset                   ();
    void                    shutdown                ( bool kill = false );
};

} // namespace Volition
#endif
