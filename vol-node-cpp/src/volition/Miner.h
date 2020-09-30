// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINER_H
#define VOLITION_MINER_H

#include <volition/MinerBase.h>

namespace Volition {

class AbstractChainRecorder;
class AbstractHashable;

//================================================================//
// MinerSearchEntry
//================================================================//
class MinerSearchEntry {
protected:

    friend class Miner;

    BlockTreeNode::ConstPtr     mSearchTarget;
    size_t                      mSearchCount;
    size_t                      mSearchLimit;
};

//================================================================//
// Miner
//================================================================//
class Miner :
    public MinerBase {
protected:
    
    map < string, MinerSearchEntry >    mSearches;
    
    //----------------------------------------------------------------//
    void                        affirmSearch                ( BlockTreeNode::ConstPtr branch );
    bool                        canExtend                   () const;
    void                        composeChain                ();
    void                        processResponses            ();
    void                        requestHeaders              ();
    void                        selectBestBranch            ();
    BlockTreeNode::ConstPtr     truncate                    ( BlockTreeNode::ConstPtr tail ) const;
    void                        updateChainRecurse          ( BlockTreeNode::ConstPtr branch );
    void                        updateSearches              ();

public:

    //----------------------------------------------------------------//
                                Miner                       ();
    virtual                     ~Miner                      ();
    void                        step                        ( bool solo = false );
};

} // namespace Volition
#endif
