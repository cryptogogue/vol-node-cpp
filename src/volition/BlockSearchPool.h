// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKSEARCHPOOL_H
#define VOLITION_BLOCKSEARCHPOOL_H

#include <volition/common.h>
#include <volition/AbstractMiningMessenger.h>
#include <volition/Accessors.h>
#include <volition/AbstractBlockTree.h>
#include <volition/FeeDistributionTable.h>
#include <volition/FeeSchedule.h>
#include <volition/CryptoKey.h>
#include <volition/Ledger.h>
#include <volition/RemoteMiner.h>
#include <volition/TransactionQueue.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/Singleton.h>
#include <volition/TransactionStatus.h>

namespace Volition {

class AbstractChainRecorder;
class AbstractConsensusInspector;
class AbstractHashable;
class BlockSearchPool;
class Miner;

//================================================================//
// BlockSearch
//================================================================//
class BlockSearch {
protected:

    friend class BlockSearchPool;
    friend class BlockSearchKey;

    static const size_t SAMPLE_SIZE     = 2;
    static const size_t MAX_RETRIES     = 256;

    friend class Miner;

    string              mTag;
    string              mHash;
    u64                 mHeight;
    set < string >      mActiveMiners;
    set < string >      mCompletedMiners;
    size_t              mRetries;

    chrono::high_resolution_clock::time_point   mT0;

public:

    //----------------------------------------------------------------//
    bool                    extendSearch                    ( BlockSearchPool& pool );
                            BlockSearch                     ();
    void                    initialize                      ( BlockTreeCursor cursor );
    void                    reset                           ();
    bool                    step                            ( BlockSearchPool& pool );
    void                    step                            ( string minderID );
};

//================================================================//
// BlockSearchKey
//================================================================//
class BlockSearchKey {
public:

    BlockSearch&    mBlockSearch;
    
    //----------------------------------------------------------------//
    bool operator < ( const BlockSearchKey& rhs ) const {
    
        if ( this->mBlockSearch.mHeight == rhs.mBlockSearch.mHeight ) return this->mBlockSearch.mHash < rhs.mBlockSearch.mHash;
        return this->mBlockSearch.mHeight < rhs.mBlockSearch.mHeight;
    }
    
    //----------------------------------------------------------------//
    BlockSearchKey ( BlockSearch& blockSearch ) :
        mBlockSearch ( blockSearch ) {
    }
};

//================================================================//
// BlockSearchPool
//================================================================//
class BlockSearchPool {
protected:

    friend class BlockSearch;
    
    static const size_t MAX_SEARCHES = 256;

    Miner&                          mMiner;
    AbstractBlockTree&              mBlockTree;
    set < BlockSearchKey >          mActiveSearches;
    map < string, BlockSearch >     mBlockSearchesByHash;
    set < BlockSearchKey >          mPendingSearches;

    //----------------------------------------------------------------//
    void                    erase                           ( string hash );

public:
    
    //----------------------------------------------------------------//
    void                    affirmBranchSearch              ( BlockTreeCursor cursor );
                            BlockSearchPool                 ( Miner& miner, AbstractBlockTree& blockTree );
    virtual                 ~BlockSearchPool                ();
    size_t                  countActiveSearches             () const;
    size_t                  countSearches                   () const;
    BlockSearch*            findBlockSearch                 ( const Digest& digest );
    void                    reportBlockSearches             () const;
    void                    update                          ();
    void                    updateBlockSearch               ( string minerID, string hash, bool found );
};

} // namespace Volition
#endif
