// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMPLECHAINRECORDER_H
#define VOLITION_SIMPLECHAINRECORDER_H

#include <volition/common.h>
#include <volition/AbstractChainRecorder.h>

namespace Volition {

class Miner;

//================================================================//
// SimpleChainRecorder
//================================================================//
class SimpleChainRecorder :
    public AbstractChainRecorder {
protected:

    static constexpr const char* BLOCK_FOLDERNAME       = "blocks";
    static constexpr const char* INDEX_FILENAME         = "index.json";

    string                  mBasePath;
    string                  mBlocksFolderPath;
    string                  mChainFolderPath;
    string                  mIndexFilePath;
    
    string                  mGenesisHash;

    //----------------------------------------------------------------//
    void                    AbstractChainRecorder_loadChain         ( Miner& miner ) const;
    void                    AbstractChainRecorder_saveChain         ( const Miner& miner );

public:

    //----------------------------------------------------------------//
                            SimpleChainRecorder         ( const Miner& miner, string path = "./" );
                            ~SimpleChainRecorder        ();
};

} // namespace Volition
#endif
