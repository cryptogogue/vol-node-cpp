// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SQLITECHAINRECORDER_H
#define VOLITION_SQLITECHAINRECORDER_H

#include <volition/common.h>
#include <sqlite3.h>
#include <volition/AbstractChainRecorder.h>

namespace Volition {

class Miner;

//================================================================//
// SQLiteChainRecorder
//================================================================//
class SQLiteChainRecorder :
    public AbstractChainRecorder {
protected:

    typedef std::function < int ( int, char**, char** )>                                SQLCallbackFunc;
    typedef std::function < void ( sqlite3_stmt* )>                                     SQLPrepareCallbackFunc;
    typedef std::function < void ( int, const map < string, int >&, sqlite3_stmt* )>    SQLRowCallbackFunc;

    sqlite3*                mDB;

    //----------------------------------------------------------------//
    void                    exec                        ( sqlite3_stmt* stmt, SQLRowCallbackFunc onRow );
    void                    exec                        ( string sql, SQLPrepareCallbackFunc onPrepare = NULL, SQLRowCallbackFunc onRow = NULL );
    sqlite3_stmt*           prepare                     ( string sql, SQLPrepareCallbackFunc onPrepare = NULL );

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
