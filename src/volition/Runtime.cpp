// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Format.h>
#include <volition/Ledger.h>
#include <volition/Runtime.h>

namespace Volition {

static const int LEVELS1 = 12;    // size of the first part of the stack
static const int LEVELS2 = 10;    // size of the second part of the stack

static const char* LUA_GLOBAL_LEDGER        = "LEDGER";
static const char* LUA_GLOBAL_SCHEMA_NAME   = "SCHEMA_NAME";
static const char* PUBLISH_FUNC_NAME        = "publish";

//----------------------------------------------------------------//
Ledger*         _get_ledger         ( lua_State* L );
string          _get_schema_name    ( lua_State* L );
int             _lua_call           ( lua_State* L, int nargs, int nresults );
int             _traceback          ( lua_State* L );

//----------------------------------------------------------------//
Ledger* _get_ledger ( lua_State* L ) {

    lua_getglobal ( L, LUA_GLOBAL_LEDGER );
    Ledger* ledger = ( Ledger* )lua_touserdata ( L, -1 );
    lua_pop ( L, 1 );
    
    return ledger;
}

//----------------------------------------------------------------//
string _get_schema_name ( lua_State* L ) {

    lua_getglobal ( L, LUA_GLOBAL_SCHEMA_NAME );
    string schemaName;
    if ( lua_isstring ( L, -1 )) {
        schemaName = lua_tostring ( L, -1 );
    }
    lua_pop ( L, 1 );
    
    return schemaName;
}

//----------------------------------------------------------------//
int _lua_call ( lua_State* L, int nargs, int nresults ) {

    int top = lua_gettop ( L );
    int errIdx = lua_gettop ( L ) - nargs;

    lua_pushcfunction ( L, _traceback );
    lua_insert ( L, errIdx );

    top = lua_gettop ( L );

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
    str.append ( "\n" );
    
    LOG_F ( INFO, "LUA: %s", str.c_str ());
    
    return 0;
}

//----------------------------------------------------------------//
int _traceback ( lua_State* L ) {
    
    if ( lua_isstring ( L, 1 )) {  // 'message' a string?
        const char* msg = lua_tostring ( L, 1 );
        LOG_F ( INFO, "LUA.ERROR: %s", msg );
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
    
    LOG_F ( INFO, "LUA.ERROR.STACKTRACE: %s", out.c_str ());

    return 0;
}

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
int Runtime::_awardAsset ( lua_State* L ) {
    
    Ledger* ledger = _get_ledger ( L );
    string schemaName = _get_schema_name ( L );
    
    assert ( ledger && ( schemaName.size () > 0 ));
    
    string accountName      = lua_tostring ( L, 1 );
    string assetName        = lua_tostring ( L, 2 );
    int quantity            = ( int )lua_tointeger ( L, 3 );

    ledger->awardAsset ( schemaName, accountName, assetName, quantity );

    return 0;
}

//================================================================//
// Runtime
//================================================================//

//----------------------------------------------------------------//
void Runtime::loadScript ( Ledger& ledger, string schemaName, string lua ) {

    assert ( this->mLuaState );
    
    lua_pushlightuserdata ( this->mLuaState, &ledger );
    lua_setglobal ( this->mLuaState, "LEDGER" );
    
    lua_pushstring ( this->mLuaState, schemaName.c_str ());
    lua_setglobal ( this->mLuaState, "SCHEMA_NAME" );
    
    luaL_loadbuffer ( this->mLuaState, lua.c_str (), lua.size (), "schema" );
    _lua_call ( this->mLuaState, 0, 0 );
}

//----------------------------------------------------------------//
void Runtime::miningReward ( string rewardName ) {
}

//----------------------------------------------------------------//
void Runtime::publish () {

    int type = lua_getglobal ( this->mLuaState, PUBLISH_FUNC_NAME );
    assert ( type == LUA_TFUNCTION );
    
    _lua_call ( this->mLuaState, 0, 0 );
}

//----------------------------------------------------------------//
void Runtime::runRule ( string ruleName, AssetIdentifier* assets, size_t nAssets ) {
}

//----------------------------------------------------------------//
Runtime::Runtime () {
    
    this->mLuaState = luaL_newstate ();
    
    // TODO: sandbox or omit this in release builds
    luaL_openlibs ( this->mLuaState );
    
    lua_pushcfunction ( this->mLuaState, _print );
    lua_setglobal ( this->mLuaState, "print" );
    
    const luaL_Reg funcs [] = {
        { "awardAsset",     _awardAsset },
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
