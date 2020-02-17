// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Asset.h>
#include <volition/AssetMethodInvocation.h>
#include <volition/Format.h>
#include <volition/LuaContext.h>

namespace Volition {

static const int LEVELS1 = 12;    // size of the first part of the stack
static const int LEVELS2 = 10;    // size of the second part of the stack

//static const char* LUA_GLOBAL_SCHEMA    = "SCHEMA";
static const char* MAIN_FUNC_NAME       = "main";

//----------------------------------------------------------------//
//LuaContext*      _get_schema         ( lua_State* L );
int             _lua_call           ( lua_State* L, int nargs, int nresults );
int             _print              ( lua_State *L );
int             _traceback          ( lua_State* L );

////----------------------------------------------------------------//
//LuaContext* _get_schema ( lua_State* L ) {
//
//    lua_getglobal ( L, LUA_GLOBAL_SCHEMA );
//    LuaContext* schemaLua = ( LuaContext* )lua_touserdata ( L, -1 );
//    lua_pop ( L, 1 );
//
//    return schemaLua;
//}

//----------------------------------------------------------------//
int _lua_call ( lua_State* L, int nargs, int nresults ) {

    int errIdx = lua_gettop ( L ) - nargs;

    lua_pushcfunction ( L, _traceback );
    lua_insert ( L, errIdx );

    int status = lua_pcall ( L, nargs, nresults, errIdx );

    if ( !status ) {
        lua_remove ( L, errIdx );
    }
    return status;
}

//----------------------------------------------------------------//
int _print ( lua_State *L ) {

    int n = lua_gettop ( L );  /* number of arguments */
    int i;
    
    string str;
    
    lua_getglobal ( L, "tostring" );
    for ( i = 1; i <= n; i++ ) {
    
        const char *s;
        size_t l;
        
        lua_pushvalue ( L, -1 );  /* function to be called */
        lua_pushvalue ( L, i );   /* value to print */
        
        lua_call ( L, 1, 1 );
        s = lua_tolstring ( L, -1, &l );  /* get result */
        
        if ( s == NULL ) {
            return luaL_error ( L, "'tostring' must return a string to 'print'" );
        }
        
        if ( i > 1 ) {
            str.append ( "\t" );
        }
        str.append ( s );
        lua_pop ( L, 1 );  /* pop result */
    }
//    str.append ( "\n" );
    
    LGN_LOG ( VOL_FILTER_ROOT, INFO, "LUA: %s", str.c_str ());
    
    return 0;
}

//----------------------------------------------------------------//
int _traceback ( lua_State* L ) {
    
    if ( lua_isstring ( L, 1 )) {  // 'message' a string?
        const char* msg = lua_tostring ( L, 1 );
        LGN_LOG ( VOL_FILTER_ROOT, INFO, "LUA.ERROR: %s", msg );
    }
    
    int firstpart = 1;  /* still before eventual `...' */
    lua_Debug ar;

    int level = 1;
    string out;

    while ( lua_getstack ( L, level++, &ar )) {

        if ( level > LEVELS1 && firstpart ) {

            if ( !lua_getstack ( L, level + LEVELS2, &ar )) {
                level--;
            }
            else {
                // too many levels
                out.append ( "\n\t..." );  /* too many levels */

                // find last levels */
                while ( lua_getstack ( L, level + LEVELS2, &ar ))
                    level++;
            }
            firstpart = 0;
            continue;
        }

        out.append ( "\n\t" );

        lua_getinfo ( L, "Snl", &ar );

        out.append ( ar.short_src );

        if ( ar.currentline > 0 ) {
            Format::write ( out, ":%d", ar.currentline );
        }

        if ( *ar.namewhat != '\0' ) {
            Format::write ( out, " in function '%s'", ar.name );
        }
        else {
            if ( *ar.what == 'm' ) {
                Format::write ( out, " in main chunk" );
            }
            else if ( *ar.what == 'C' || *ar.what == 't' ) {
                Format::write ( out, " ?" );
            }
            else {
                Format::write ( out, " in function <%s:%d>", ar.short_src, ar.linedefined );
            }
        }
    }

    out.append ( "\n" );
    
    LGN_LOG ( VOL_FILTER_ROOT, INFO, "LUA.ERROR.STACKTRACE: %s", out.c_str ());

    return 0;
}

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
int LuaContext::_awardAsset ( lua_State* L ) {

    lua_getfield ( L, LUA_REGISTRYINDEX, CONTEXT_KEY );
    LuaContext* self = ( LuaContext* )lua_touserdata ( L, 1 );
    assert ( self );
    lua_pop ( L, 1 );
    
    string accountName      = lua_tostring ( L, 2 );
    string assetType        = lua_tostring ( L, 3 );
    size_t quantity         = ( size_t )lua_tointeger ( L, 4 );

    self->mLedger.awardAsset ( self->mSchema, accountName, assetType, quantity );

    return 0;
}

//----------------------------------------------------------------//
int LuaContext::_getEntropy ( lua_State* L ) {
    UNUSED ( L );

    return 0;
}

//----------------------------------------------------------------//
int LuaContext::_revokeAsset ( lua_State* L ) {
    UNUSED ( L );

    return 0;
}

//================================================================//
// Schema
//================================================================//

//----------------------------------------------------------------//
bool LuaContext::invoke ( string accountName ) {

    int type = lua_getglobal ( this->mLuaState, MAIN_FUNC_NAME );
    assert ( type == LUA_TFUNCTION );

    // push the account name
    lua_pushstring ( this->mLuaState, accountName.c_str ());
    
    // call the method
    _lua_call ( this->mLuaState, 1, 0 );
    return true; // TODO: handle error
}

//----------------------------------------------------------------//
bool LuaContext::invoke ( string accountName, const AssetMethod& method, const AssetMethodInvocation& invocation ) {

    // get all the assets for the asset params
    map < string, shared_ptr < Asset >> assets;
    AssetMethodInvocation::AssetParams::const_iterator assetParamIt = invocation.mAssetParams.cbegin ();
    for ( ; assetParamIt != invocation.mAssetParams.cend (); ++assetParamIt ) {
    
        string paramName = assetParamIt->first;
        Asset::Index assetID = assetParamIt->second;
    
        shared_ptr < Asset > asset = this->mLedger.getAsset ( this->mSchema, assetID );
        if ( !( asset && method.qualifyAssetArg ( paramName, *asset ))) return false;
        
        assets [ paramName ] = asset;
    }

    // get the main
    int type = lua_getglobal ( this->mLuaState, MAIN_FUNC_NAME );
    assert ( type == LUA_TFUNCTION );

    // push the account name
    lua_pushstring ( this->mLuaState, accountName.c_str ());

    // push the asset params table
    lua_newtable ( this->mLuaState );
    
    map < string, shared_ptr < Asset >>::const_iterator assetIt = assets.cbegin ();
    for ( ; assetIt != assets.cend (); ++assetIt ) {
        lua_pushstring ( this->mLuaState, assetIt->first.c_str ());
        this->push ( *assetIt->second );
        lua_settable ( this->mLuaState, -3 );
    }
    
    // push the const params table
    lua_newtable ( this->mLuaState );
    
    AssetMethodInvocation::ConstParams::const_iterator constParamIt = invocation.mConstParams.cbegin ();
    for ( ; constParamIt != invocation.mConstParams.cend (); ++constParamIt ) {
        lua_pushstring ( this->mLuaState, constParamIt->first.c_str ());
        this->push ( constParamIt->second );
        lua_settable ( this->mLuaState, -3 );
    }

    // call the method
    _lua_call ( this->mLuaState, 3, 0 );
    return true; // TODO: handle error
}

//----------------------------------------------------------------//
LuaContext::LuaContext ( Ledger& ledger, const Schema& schema, string lua ) :
    mLedger ( ledger ),
    mSchema ( schema ) {
    
    this->mLuaState = luaL_newstate ();
    
    // TODO: sandbox or omit this in release builds
    luaL_openlibs ( this->mLuaState );
    
    this->registerFunc ( "awardAsset",      _awardAsset );
    this->registerFunc ( "getEntropy",      _getEntropy );
    this->registerFunc ( "print",           _print );
    this->registerFunc ( "revokeAsset",     _revokeAsset );
    
//    const luaL_Reg funcs [] = {
//        { "awardAsset",     _awardAsset },
//        { "getEntropy",     _getEntropy },
//        { "revokeAsset",    _revokeAsset },
//        { NULL, NULL }
//    };
//
//    luaL_newlib ( this->mLuaState, funcs );
//    lua_setglobal ( this->mLuaState, "schema" );
    
//    lua_pushlightuserdata ( this->mLuaState, this );
//    lua_setglobal ( this->mLuaState, LUA_GLOBAL_SCHEMA );
    
    // set the ledger
    lua_pushlightuserdata ( this->mLuaState, this );
    lua_setfield ( this->mLuaState, LUA_REGISTRYINDEX, CONTEXT_KEY );
    
    luaL_loadbuffer ( this->mLuaState, lua.c_str (), lua.size (), "main" );
    _lua_call ( this->mLuaState, 0, 0 );
}

//----------------------------------------------------------------//
LuaContext::~LuaContext () {
    
    lua_close ( this->mLuaState );
}

//----------------------------------------------------------------//
void LuaContext::miningReward ( Ledger& ledger, string rewardName ) {
    UNUSED ( ledger );
    UNUSED ( rewardName );
}

//----------------------------------------------------------------//
void LuaContext::push ( const Asset& asset ) {

    lua_newtable ( this->mLuaState );
    
    lua_pushstring ( this->mLuaState, "type" );
    lua_pushstring ( this->mLuaState, asset.mType.c_str ());
    lua_settable ( this->mLuaState, -3 );
    
    lua_pushstring ( this->mLuaState, "index" );
    lua_pushnumber ( this->mLuaState, ( double )asset.mIndex );
    lua_settable ( this->mLuaState, -3 );
    
    lua_pushstring ( this->mLuaState, "owner" );
    lua_pushstring ( this->mLuaState, asset.mOwner.c_str ());
    lua_settable ( this->mLuaState, -3 );
    
    lua_pushstring ( this->mLuaState, "fields" );
    lua_newtable ( this->mLuaState );
    
    Asset::Fields::const_iterator fieldIt = asset.mFields.cbegin ();
    for ( ; fieldIt != asset.mFields.cend (); ++fieldIt ) {
        lua_pushstring ( this->mLuaState, fieldIt->first.c_str ());
        this->push ( fieldIt->second );
        lua_settable ( this->mLuaState, -3 );
    }
    lua_settable ( this->mLuaState, -3 );
}

//----------------------------------------------------------------//
void LuaContext::push ( const AssetFieldValue& value ) {

    switch ( value.getType ()) {
        case AssetFieldValue::Type::TYPE_BOOL:
            lua_pushboolean ( this->mLuaState, value.strictBoolean ());
            break;
        case AssetFieldValue::Type::TYPE_NUMBER:
            lua_pushnumber ( this->mLuaState, value.strictNumber ());
            break;
        case AssetFieldValue::Type::TYPE_STRING:
            lua_pushstring ( this->mLuaState, value.strictString ().c_str ());
            break;
        default:
            lua_pushnil ( this->mLuaState );
    }
}

//----------------------------------------------------------------//
void LuaContext::registerFunc ( string name, lua_CFunction func ) {

    lua_pushcfunction ( this->mLuaState, func );
    lua_setglobal ( this->mLuaState, name.c_str ());
}

} // namespace Volition
