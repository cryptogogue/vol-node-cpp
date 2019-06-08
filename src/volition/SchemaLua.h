// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SCHEMALUA_H
#define VOLITION_SCHEMALUA_H

#include <volition/common.h>
#include <volition/Ledger.h>
#include <volition/Schema.h>

namespace Volition {

//================================================================//
// SchemaLua
//================================================================//
class SchemaLua {
private:

    friend class Ledger;

    Schema          mSchema;
    lua_State*      mLuaState;

    //----------------------------------------------------------------//
    static int      _awardAsset                 ( lua_State* L );
    static int      _getEntropy                 ( lua_State* L );
    static int      _revokeAsset                ( lua_State* L );

public:

    //----------------------------------------------------------------//
    void            miningReward                ( Ledger& ledger, string rewardName );
    void            publish                     ( Ledger& ledger );
    bool            runMethod                   ( Ledger& ledger, string methodName, u64 weight, u64 maturity, const string* assets, size_t nAssets );
                    SchemaLua                   ( const Schema& schema );
                    ~SchemaLua                  ();
};

} // namespace Volition
#endif
