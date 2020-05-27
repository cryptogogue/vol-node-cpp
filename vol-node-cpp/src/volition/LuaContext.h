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
    
    static constexpr const char* CONTEXT_KEY    = "vol.context";

    ConstOpt < Ledger >     mLedger;
    const Schema&           mSchema;
    lua_State*              mLuaState;

    //----------------------------------------------------------------//
    static int              _awardAsset             ( lua_State* L );
    static int              _getEntropy             ( lua_State* L );
    static int              _getDefinitionField     ( lua_State* L );
    static int              _randomAward            ( lua_State* L );
    static int              _resetAssetField        ( lua_State* L );
    static int              _revokeAsset            ( lua_State* L );
    static int              _setAssetField          ( lua_State* L );

    //----------------------------------------------------------------//
    static LuaContext&      getSelf                 ( lua_State* L );
    void                    push                    ( const Asset& asset );
    void                    push                    ( const AssetFieldValue& value );
    void                    registerFunc            ( string name, lua_CFunction func );

public:

    //----------------------------------------------------------------//
    LedgerResult            compile                 ( const AssetMethod& method );
    LedgerResult            invoke                  ( string accountName, const AssetMethod& method, const AssetMethodInvocation& invocation );
                            LuaContext              ( ConstOpt < Ledger > ledger, const Schema& schema );
                            ~LuaContext             ();
    void                    miningReward            ( Ledger& ledger, string rewardName );
    void                    publish                 ( Ledger& ledger );
};

} // namespace Volition
#endif
