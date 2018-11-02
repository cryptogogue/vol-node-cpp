// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Ledger.h>
#include <volition/Runtime.h>

namespace Volition {

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
int Runtime::_createAsset ( lua_State* L ) {

    return 0;
}

//================================================================//
// Runtime
//================================================================//

//----------------------------------------------------------------//
void Runtime::loadScript ( string lua ) {

    assert ( this->mLuaState );
    luaL_dostring ( this->mLuaState, lua.c_str ());
}

//----------------------------------------------------------------//
void Runtime::miningReward ( Ledger& ledger, string rewardName ) {
}

//----------------------------------------------------------------//
void Runtime::publish ( Ledger& ledger ) {

     assert ( this->mLuaState );

    int type = lua_getglobal ( this->mLuaState, PUBLISH_FUNC_NAME );
    assert ( type == LUA_TFUNCTION );
    
    lua_pcall ( this->mLuaState, 0, 0, 0 );
}

//----------------------------------------------------------------//
void Runtime::runRule ( Ledger& ledger, string ruleName, AssetIdentifier* assets, size_t nAssets ) {
}

//----------------------------------------------------------------//
Runtime::Runtime () {
    
    this->mLuaState = luaL_newstate ();
    
    luaL_openlibs ( this->mLuaState );
    
    const luaL_Reg funcs [] = {
        { "createAsset",    _createAsset },
        { NULL, NULL }
    };

    luaL_newlib ( this->mLuaState, funcs );
    lua_setglobal ( this->mLuaState, "ledger" );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
