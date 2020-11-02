// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SQLITECHAINRECORDER_H
#define VOLITION_SQLITECHAINRECORDER_H

#include <volition/common.h>
#include <sqlite3.h>
#include <volition/AbstractChainRecorder.h>
#include <volition/SQLite.h>

namespace Volition {

class Miner;

//================================================================//
// SQLiteChainRecorder
//================================================================//
class SQLiteChainRecorder :
    public AbstractChainRecorder {
protected:

    SQLite                  mDB;

    //----------------------------------------------------------------//
    void                    AbstractChainRecorder_loadChain         ( Miner& miner );
    void                    AbstractChainRecorder_loadConfig        ( MinerConfig& minerConfig );
    void                    AbstractChainRecorder_reset             ();
    void                    AbstractChainRecorder_saveChain         ( const Miner& miner );
    void                    AbstractChainRecorder_saveConfig        ( const MinerConfig& minerConfig );

public:

    //----------------------------------------------------------------//
                            SQLiteChainRecorder         ( const Miner& miner, string path = "." );
                            ~SQLiteChainRecorder        ();
};

} // namespace Volition
#endif
