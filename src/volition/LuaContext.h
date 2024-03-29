// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LUACONTEXT_H
#define VOLITION_LUACONTEXT_H

#include <volition/common.h>
#include <volition/Ledger.h>
#include <volition/Psuedorandom.h>
#include <volition/Schema.h>

namespace Volition {

class AssetMethodInvocation;

//================================================================//
// LuaContext
//================================================================//
class LuaContext {
private:

    friend class AbstractLedger;
    
    typedef  vector < unsigned char > Buffer;
    
    static constexpr const char* CONTEXT_KEY    = "vol.context";

    ConstOpt < AbstractLedger >     mLedger;
    time_t                          mTime;
    lua_State*                      mLuaState;
    LedgerResult                    mResult;
    Psuedorandom                    mPRNG;

    //----------------------------------------------------------------//
    static int              _awardAsset             ( lua_State* L );
    static int              _awardDeck              ( lua_State* L );
    static int              _awardVOL               ( lua_State* L );
    static int              _bytesToScalar          ( lua_State* L );
    static int              _getDefinitionField     ( lua_State* L );
    static int              _getEntropy             ( lua_State* L );
    static int              _randomAward            ( lua_State* L );
    static int              _randomDouble           ( lua_State* L );
    static int              _randomInt32            ( lua_State* L );
    static int              _resetAssetField        ( lua_State* L );
    static int              _resetAssetFields       ( lua_State* L );
    static int              _revokeAsset            ( lua_State* L );
    static int              _seedRandom             ( lua_State* L );
    static int              _setAssetField          ( lua_State* L );
    static int              _setStamp               ( lua_State* L );

    //----------------------------------------------------------------//
    AccountID               checkAccountName        ( string accountName );
    AssetID::Index          checkAssetID            ( string assetID );
    bool                    checkAssetType          ( string assetType );
    bool                    checkDeckOrSet          ( string deckName );
    const AssetDefinition*  checkDefinition         ( string definitionName );
    AssetFieldDefinition    checkDefinitionField    ( const AssetDefinition& definition, string fieldName );
    LedgerResult            compile                 ( string lua );
    static Buffer           getBuffer               ( lua_State* L, int idx );
    static AssetFieldValue  getFieldValue           ( lua_State* L, int idx );
    static LuaContext&      getSelf                 ( lua_State* L );
    void                    push                    ( const Asset& asset );
    void                    push                    ( const AssetFieldValue& value );
    void                    registerFunc            ( string name, lua_CFunction func );
    void                    setResult               ( LedgerResult result );

public:

    //----------------------------------------------------------------//
    LedgerResult            invoke                  ( string accountName, const AssetMethod& method, const AssetMethodInvocation& invocation );
    LedgerResult            invoke                  ( string accountName, string rewardName );
                            LuaContext              ( ConstOpt < AbstractLedger > ledger, time_t time );
                            ~LuaContext             ();
};

} // namespace Volition
#endif
