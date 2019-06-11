// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LUACONTEXT_H
#define VOLITION_LUACONTEXT_H

#include <volition/common.h>
#include <volition/Ledger.h>
#include <volition/Schema.h>

namespace Volition {

class AssetMethodInvocation;

//================================================================//
// LuaContext
//================================================================//
class LuaContext {
private:

    friend class Ledger;

    Schema          mSchema;
    lua_State*      mLuaState;

    //----------------------------------------------------------------//
    static int      _awardAsset                 ( lua_State* L );
    static int      _getEntropy                 ( lua_State* L );
    static int      _revokeAsset                ( lua_State* L );

    //----------------------------------------------------------------//
    void            push                        ( const Asset& asset );
    void            push                        ( const AssetFieldValue& value );
    void            registerFunc                ( string name, lua_CFunction func );

public:

    //----------------------------------------------------------------//
    bool            invoke                      ( Ledger& ledger, string accountName );
    bool            invoke                      ( Ledger& ledger, string accountName, const AssetMethod& method, const AssetMethodInvocation& invocation );
    void            miningReward                ( Ledger& ledger, string rewardName );
    void            publish                     ( Ledger& ledger );
    bool            runMethod                   ( Ledger& ledger, string methodName, u64 weight, u64 maturity, const string* assets, size_t nAssets );
                    LuaContext                  ( string lua );
                    ~LuaContext                 ();
};

} // namespace Volition
#endif
