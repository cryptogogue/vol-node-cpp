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
    time_t                  mTime;
    lua_State*              mLuaState;
    LedgerResult            mResult;

    //----------------------------------------------------------------//
    static int              _awardAsset             ( lua_State* L );
    static int              _awardDeck              ( lua_State* L );
    static int              _awardVOL               ( lua_State* L );
    static int              _getEntropy             ( lua_State* L );
    static int              _getDefinitionField     ( lua_State* L );
    static int              _randomAward            ( lua_State* L );
    static int              _resetAssetField        ( lua_State* L );
    static int              _revokeAsset            ( lua_State* L );
    static int              _setAssetField          ( lua_State* L );

    //----------------------------------------------------------------//
    AccountID               checkAccountName        ( string accountName );
    AssetID::Index          checkAssetID            ( string assetID );
    bool                    checkAssetType          ( string assetType );
    bool                    checkDeckOrSet          ( string deckName );
    const AssetDefinition*  checkDefinition         ( string definitionName );
    AssetFieldDefinition    checkDefinitionField    ( const AssetDefinition& definition, string fieldName );
    LedgerResult            compile                 ( string lua );
    static LuaContext&      getSelf                 ( lua_State* L );
    void                    push                    ( const Asset& asset );
    void                    push                    ( const AssetFieldValue& value );
    void                    registerFunc            ( string name, lua_CFunction func );
    void                    setResult               ( LedgerResult result );

public:

    //----------------------------------------------------------------//
    LedgerResult            invoke                  ( string accountName, const AssetMethod& method, const AssetMethodInvocation& invocation );
    LedgerResult            invoke                  ( string accountName, string rewardName );
                            LuaContext              ( ConstOpt < Ledger > ledger, const Schema& schema, time_t time );
                            ~LuaContext             ();
    void                    miningReward            ( Ledger& ledger, string rewardName );
    void                    publish                 ( Ledger& ledger );
};

} // namespace Volition
#endif
