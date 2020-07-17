// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AccountODBM.h>
#include <volition/Asset.h>
#include <volition/AssetMethodInvocation.h>
#include <volition/AssetODBM.h>
#include <volition/Format.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/LuaContext.h>

namespace Volition {

static const int LEVELS1 = 12;    // size of the first part of the stack
static const int LEVELS2 = 10;    // size of the second part of the stack

//static const char* LUA_GLOBAL_SCHEMA    = "SCHEMA";
static const char* MAIN_FUNC_NAME       = "main";

//----------------------------------------------------------------//
LedgerResult    _lua_call           ( lua_State* L, int nargs, int nresults );
int             _print              ( lua_State *L );
int             _traceback          ( lua_State* L );
string          _to_string          ( lua_State* L, int idx );

//----------------------------------------------------------------//
LedgerResult _lua_call ( lua_State* L, int nargs, int nresults ) {

    int errIdx = lua_gettop ( L ) - nargs;

    lua_pushcfunction ( L, _traceback );
    lua_insert ( L, errIdx );

    int status = lua_pcall ( L, nargs, nresults, errIdx );

    if ( !status ) {
        lua_remove ( L, errIdx );
    }
    else {
        string error = lua_tostring ( L, -1 );
        lua_pop ( L, 1 );
        return error;
    }
    return true;
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
    
    string out;
    
    if ( lua_isstring ( L, 1 )) {  // 'message' a string?
        const char* msg = lua_tostring ( L, 1 );
        Format::write ( out, "%s\n", msg );
    }
    
    int firstpart = 1;  /* still before eventual `...' */
    lua_Debug ar;

    int level = 1;
    

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
    
    LGN_LOG ( VOL_FILTER_ROOT, INFO, "LUA ERROR: %s", out.c_str ());
    lua_pushstring ( L, out.c_str ());

    return 1;
}

//----------------------------------------------------------------//
string _to_string ( lua_State* L, int idx ) {

    cc8* str = lua_tostring ( L, idx );
    return str ? str : "";
}

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
int LuaContext::_awardAsset ( lua_State* L ) {
    LuaContext& self = LuaContext::getSelf ( L );
    
    string accountName      = _to_string ( L, 1 );
    string assetType        = _to_string ( L, 2 );
    size_t quantity         = ( size_t )lua_tointeger ( L, 3 );

    Account::Index accountIndex = self.checkAccountName ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return 0;
    if ( !self.checkAssetType ( assetType )) return 0;

    self.setResult ( self.mLedger->awardAssets ( self.mSchema, accountIndex, assetType, quantity, self.mTime ));
    return 0;
}

//----------------------------------------------------------------//
int LuaContext::_awardDeck ( lua_State* L ) {
    LuaContext& self = LuaContext::getSelf ( L );
    
    string accountName      = _to_string ( L, 1 );
    string setOrDeckName    = _to_string ( L, 2 );

    Account::Index accountIndex = self.checkAccountName ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return 0;
    if ( !self.checkDeckOrSet ( setOrDeckName )) return 0;

    self.setResult ( self.mLedger->awardDeck ( self.mSchema, accountIndex, setOrDeckName, self.mTime ));
    return 0;
}

//----------------------------------------------------------------//
int LuaContext::_awardVOL ( lua_State* L ) {
    LuaContext& self = LuaContext::getSelf ( L );
    
    string accountName      = _to_string ( L, 1 );
    size_t amount           = ( size_t )lua_tointeger ( L, 2 );

    Account::Index accountIndex = self.checkAccountName ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return 0;

    self.setResult ( self.mLedger->awardVOL ( accountIndex, amount ));
    return 0;
}

//----------------------------------------------------------------//
int LuaContext::_getDefinitionField ( lua_State* L ) {
    LuaContext& self = LuaContext::getSelf ( L );

    string definitionName   = _to_string ( L, 1 );
    string fieldName        = _to_string ( L, 2 );

    const AssetDefinition* definition = self.checkDefinition ( definitionName );
    if ( !definition ) return 0;

    AssetFieldDefinition field = self.checkDefinitionField ( *definition, fieldName );
    if ( !field.isValid ()) return 0;
    
    switch ( field.getType ()) {
    
        case AssetFieldValue::Type::TYPE_BOOL:
            lua_pushboolean ( L, field.strictBoolean ());
            break;
            
        case AssetFieldValue::Type::TYPE_NUMBER:
            lua_pushnumber ( L, field.strictNumber ());
            break;
            
        case AssetFieldValue::Type::TYPE_STRING:
            lua_pushstring ( L, field.strictString ().c_str ());
            break;
            
        default:
            assert ( false );
            break;
    }
    lua_pushboolean ( L, field.mMutable );
    return 2;
}

//----------------------------------------------------------------//
int LuaContext::_getEntropy ( lua_State* L ) {
    UNUSED ( L );

    return 0;
}

//----------------------------------------------------------------//
int LuaContext::_randomAward ( lua_State* L ) {
    LuaContext& self = LuaContext::getSelf ( L );

    string accountName      = _to_string ( L, 1 );
    string setOrDeckName    = _to_string ( L, 2 );
    string seed             = _to_string ( L, 3 );
    size_t quantity         = ( size_t )lua_tointeger ( L, 4 );

    Account::Index accountIndex = self.checkAccountName ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return 0;
    if ( !self.checkDeckOrSet ( setOrDeckName )) return 0;

    self.setResult ( self.mLedger->awardAssetsRandom ( self.mSchema, accountIndex, setOrDeckName, seed, quantity, self.mTime ));
    return 0;
}

//----------------------------------------------------------------//
int LuaContext::_resetAssetField ( lua_State* L ) {
    LuaContext& self = LuaContext::getSelf ( L );
    Ledger& ledger = self.mLedger;

    string assetID          = _to_string ( L, 1 );
    string fieldName        = _to_string ( L, 2 );

    AssetID::Index assetindex = self.checkAssetID ( assetID );
    if ( assetindex == AssetID::NULL_INDEX ) return 0;

    self.setResult ( ledger.resetAssetFieldValue ( self.mSchema, assetindex, fieldName, self.mTime ));
    return 0;
}

//----------------------------------------------------------------//
int LuaContext::_revokeAsset ( lua_State* L ) {
    LuaContext& self = LuaContext::getSelf ( L );
    Ledger& ledger = self.mLedger;

    string assetID          = lua_tostring ( L, 1 );

    AssetID::Index assetindex = self.checkAssetID ( assetID );
    if ( assetindex == AssetID::NULL_INDEX ) return 0;

    self.setResult ( ledger.revokeAsset ( assetindex, self.mTime ));
    return 0;
}

//----------------------------------------------------------------//
int LuaContext::_setAssetField ( lua_State* L ) {
    LuaContext& self = LuaContext::getSelf ( L );
    Ledger& ledger = self.mLedger;

    string assetID          = _to_string ( L, 1 );
    string fieldName        = _to_string ( L, 2 );

    AssetID::Index assetindex = self.checkAssetID ( assetID );
    if ( assetindex == AssetID::NULL_INDEX ) return 0;
    
    AssetFieldValue value;
    
    switch ( lua_type ( L, 3 )) {
    
        case LUA_TBOOLEAN:
            value = lua_toboolean ( L, 3 ) ? true : false;
            break;

        case LUA_TNUMBER:
            value = lua_tonumber ( L, 3 );
            break;

        case LUA_TSTRING:
            value = lua_tostring ( L, 3 );
            break;
    }
    
    if ( !value.isValid ()) {
        self.setResult ( "Invalid field value." );
    }
    else {
        self.setResult ( ledger.setAssetFieldValue ( self.mSchema, assetindex, fieldName, value, self.mTime ));
    }
    return 0;
}

//================================================================//
// Schema
//================================================================//

//----------------------------------------------------------------//
Account::Index LuaContext::checkAccountName ( string accountName ) {

    if ( accountName.size () == 0 ) {
        this->setResult ( "Missing account name." );
        return Account::NULL_INDEX;
    }

    Account::Index accountIndex = this->mLedger->getAccountIndex ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) {
        this->setResult ( Format::write ( "Account name '%s' not found.", accountName.c_str ()));
    }
    return accountIndex;
}

//----------------------------------------------------------------//
AssetID::Index LuaContext::checkAssetID ( string assetID ) {

    if ( assetID.size () == 0 ) {
        this->setResult ( "Missing asset ID." );
        return AssetID::NULL_INDEX;
    }

    AssetID::Index assetIndex = this->mLedger->getAssetID ( assetID );
    if ( assetIndex == AssetID::NULL_INDEX ) {
        this->setResult ( Format::write ( "Asset not found for '%s'.", assetID.c_str ()));
    }
    return assetIndex;
}

//----------------------------------------------------------------//
bool LuaContext::checkAssetType ( string assetType ) {

    if ( assetType.size () == 0 ) {
        this->setResult ( "Missing asset type." );
        return false;
    }
    
    if ( !this->mSchema.hasAssetType ( assetType )) {
        this->setResult ( Format::write ( "Asset type '%s' not found.", assetType.c_str ()));
        return false;
    }
    return true;
}

//----------------------------------------------------------------//
bool LuaContext::checkDeckOrSet ( string deckName ) {

    if ( !this->mSchema.getDeck ( deckName )) {
        this->setResult ( Format::write ( "Deck '%s' not found.", deckName.c_str ()));
        return false;
    }
    return true;
}

//----------------------------------------------------------------//
const AssetDefinition* LuaContext::checkDefinition ( string definitionName ) {

    if ( definitionName.size () == 0 ) {
        this->setResult ( "Missing definition name." );
        return NULL;
    }

    const AssetDefinition* definition = this->mSchema.getDefinitionOrNull ( definitionName );
    if ( !definition ) {
        this->setResult ( Format::write ( "Definition '%s' not found.", definitionName.c_str ()));
    }
    return definition;
}

//----------------------------------------------------------------//
AssetFieldDefinition LuaContext::checkDefinitionField ( const AssetDefinition& definition, string fieldName ) {

    AssetFieldDefinition field;

    if ( fieldName.size () == 0 ) {
        this->setResult ( "Missing field name." );
        return field;
    }
    
    field = definition.getField ( fieldName );
    
    if ( !field.isValid ()) {
        this->setResult ( Format::write ( "Missing or invalid field '%s'.", fieldName.c_str ()));
    }
    return field;
}

//----------------------------------------------------------------//
LedgerResult LuaContext::compile ( const AssetMethod& method ) {

    luaL_loadbuffer ( this->mLuaState, method.mLua.c_str (), method.mLua.size (), "main" );
    return _lua_call ( this->mLuaState, 0, 0 );
}

//----------------------------------------------------------------//
LuaContext& LuaContext::getSelf ( lua_State* L ) {

    lua_getfield ( L, LUA_REGISTRYINDEX, CONTEXT_KEY );
    LuaContext* self = ( LuaContext* )lua_touserdata ( L, -1 );
    assert ( self );
    lua_pop ( L, 1 );
    
    return *self;
}

//----------------------------------------------------------------//
LedgerResult LuaContext::invoke ( string accountName, const AssetMethod& method, const AssetMethodInvocation& invocation ) {

    LedgerResult result = this->compile ( method );
    if ( !result ) {
        return result;
    }

    // get all the assets for the asset params
    map < string, shared_ptr < const Asset >> assets;
    AssetMethodInvocation::AssetParams::const_iterator assetParamIt = invocation.mAssetParams.cbegin ();
    for ( ; assetParamIt != invocation.mAssetParams.cend (); ++assetParamIt ) {
    
        string paramName = assetParamIt->first;
        AssetID::Index assetID = assetParamIt->second;
    
        assets [ paramName ] = this->mLedger->getAsset ( this->mSchema, assetID );
    }
    if ( !method.checkInvocation ( assets )) return false;

    // get the main
    int type = lua_getglobal ( this->mLuaState, MAIN_FUNC_NAME );
    assert ( type == LUA_TFUNCTION );

    // push the account name
    lua_pushstring ( this->mLuaState, accountName.c_str ());

    // push the asset params table
    lua_newtable ( this->mLuaState );
    
    map < string, shared_ptr < const Asset >>::const_iterator assetIt = assets.cbegin ();
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
    this->mResult = true;
    result = _lua_call ( this->mLuaState, 3, 0 );
    return result ? this->mResult : result;
}

//----------------------------------------------------------------//
LuaContext::LuaContext ( ConstOpt < Ledger > ledger, const Schema& schema, time_t time ) :
    mLedger ( ledger ),
    mSchema ( schema ),
    mTime ( time ),
    mResult ( true ) {
    
    this->mLuaState = luaL_newstate ();
    
    // TODO: sandbox or omit this in release builds
    luaL_openlibs ( this->mLuaState );
    
    this->registerFunc ( "awardAsset",              _awardAsset );
    this->registerFunc ( "awardDeck",               _awardDeck );
    this->registerFunc ( "awardVOL",                _awardVOL );
    this->registerFunc ( "getEntropy",              _getEntropy );
    this->registerFunc ( "getDefinitionField",      _getDefinitionField );
    this->registerFunc ( "print",                   _print );
    this->registerFunc ( "randomAward",             _randomAward );
    this->registerFunc ( "resetAssetField",         _resetAssetField );
    this->registerFunc ( "revokeAsset",             _revokeAsset );
    this->registerFunc ( "setAssetField",           _setAssetField );
    
    // set the ledger
    lua_pushlightuserdata ( this->mLuaState, this );
    lua_setfield ( this->mLuaState, LUA_REGISTRYINDEX, CONTEXT_KEY );
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
    
    lua_pushstring ( this->mLuaState, "assetID" );
    lua_pushstring ( this->mLuaState, (( string )asset.mAssetID ).c_str ());
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

//----------------------------------------------------------------//
void LuaContext::setResult ( LedgerResult result ) {

    // only overwrite if no previous error
    if ( this->mResult ) {
        this->mResult = result;
    }
}

} // namespace Volition
