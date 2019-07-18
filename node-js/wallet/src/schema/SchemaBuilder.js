/* eslint-disable no-whitespace-before-property */

import { assert }       from '../util/assert';
import fs               from 'fs';

const MUTABLE           = 'MUTABLE';
const NUMBER            = 'NUMBER';
const STRING            = 'STRING';

const TYPE_BOOLEAN      = 'BOOLEAN';
const TYPE_NUMERIC      = 'NUMERIC';
const TYPE_STRING       = 'STRING';
const TYPE_UNDEFINED    = 'UNDEFINED';

const MEDIA_AUDIO       = 'MEDIA_AUDIO';
const MEDIA_IMAGE       = 'MEDIA_IMAGE';
const MEDIA_TEXT        = 'MEDIA_TEXT';
const MEDIA_VIDEO       = 'MEDIA_VIDEO';

const SCHEMA_BUILDER_ADDING_SCHEMA                  = 'SCHEMA_BUILDER_ADDING_SCHEMA';
const SCHEMA_BUILDER_ADDING_ASSET_DEFINITION        = 'ADDING_ASSET_DEFINITION';
const SCHEMA_BUILDER_ADDING_ASSET_DEFINITION_FIELD  = 'ADDING_ASSET_DEFINITION_FIELD';
const SCHEMA_BUILDER_ADDING_METHOD                  = 'ADDING_ASSET_METHOD';

//----------------------------------------------------------------//
// function jsonEscape ( str ) {
//     return str
//         .replace ( /\\n/g, "\\n" )
//         .replace ( /\\'/g, "\\'" )
//         .replace ( /\\"/g, '\\"' )
//         .replace ( /\\&/g, "\\&" )
//         .replace ( /\\r/g, "\\r" )
//         .replace ( /\\t/g, "\\t" )
//         .replace ( /\\b/g, "\\b" )
//         .replace ( /\\f/g, "\\f" );
// }

//----------------------------------------------------------------//
function makeAssetFieldValue ( value ) {
    
    let type = TYPE_UNDEFINED;

    switch ( typeof ( value )) {
        case 'boolean':
            type = TYPE_BOOLEAN;
            break;
        case 'number':
            type = TYPE_NUMERIC;
            break;
        case 'string':
            type = TYPE_STRING;
            break;
        default:
            assert ( false );
    }

    return {
        type:       type,
        value:      value,
    }
}

//----------------------------------------------------------------//
function makeBinaryOp ( opname ) {
    return ( left, right ) => {
        return {
            op:         opname,
            left:       wrapLiteral ( left ),
            right:      wrapLiteral ( right ),
        };
    };
}

//----------------------------------------------------------------//
function makeConstOp ( opname ) {
    return ( value ) => {
        return {
            op:         opname,
            const:      makeAssetFieldValue ( value ),
        };
    };
}

//----------------------------------------------------------------//
// function makeFuncOp ( opname ) {
//     return ( ...args ) => {

//         let cleanArgs = [];

//         args.forEach ( function ( arg ) {

//             let argType = typeof ( arg );

//             if ( argType != 'object' ) {
//                 arg = op.CONST ( arg );
//             }
//             cleanArgs.push ( arg );
//         });

//         return {
//             op:         opname,
//             args:       cleanArgs,
//         };
//     };
// }

//----------------------------------------------------------------//
function makeIndexOp ( opname ) {
    return ( arg0, arg1 ) => {

        // index args are always assumed to be string literals for now.
        // can add support for operator args later. if we need it.

        if ( arg0 && arg1 ) {
            return {
                op:         opname,
                paramID:    arg0,
                value:      arg1,
            };
        }
        return {
            op:         opname,
            value:      arg0,
        };
    };
}

//----------------------------------------------------------------//
function makeUnaryOp ( opname ) {
    return ( param ) => {
        return {
            op:         opname,
            param:      wrapLiteral ( param ),
        };
    };
}

//----------------------------------------------------------------//
function wrapLiteral ( param ) {

    return ( typeof ( param ) === 'object' ) && param || op.CONST ( param );
}

//----------------------------------------------------------------//
export const op = {
    ADD:                makeBinaryOp    ( 'ADD' ),
    AND:                makeBinaryOp    ( 'AND' ),
    ASSET_TYPE:         makeIndexOp     ( 'ASSET_TYPE' ),
    CONST:              makeConstOp     ( 'CONST' ),
    DIV:                makeBinaryOp    ( 'DIV' ),
    EQUAL:              makeBinaryOp    ( 'EQUAL' ),
    FIELD:              makeIndexOp     ( 'FIELD' ),
    GREATER:            makeBinaryOp    ( 'GREATER' ),
    GREATER_OR_EQUAL:   makeBinaryOp    ( 'GREATER_OR_EQUAL' ),
    KEYWORD:            makeBinaryOp    ( 'KEYWORD' ),
    LESS:               makeBinaryOp    ( 'LESS' ),
    LESS_OR_EQUAL:      makeBinaryOp    ( 'LESS_OR_EQUAL' ),
    MOD:                makeBinaryOp    ( 'MOD' ),
    MUL:                makeBinaryOp    ( 'MUL' ),
    NOT:                makeUnaryOp     ( 'NOT' ),
    NOT_EQUAL:          makeBinaryOp    ( 'NOT_EQUAL' ),
    OR:                 makeBinaryOp    ( 'OR' ),
    SUB:                makeBinaryOp    ( 'SUB' ),
    XOR:                makeBinaryOp    ( 'XOR' ),
}

//================================================================//
// SchemaBuilder
//================================================================//
class SchemaBuilder {

    //----------------------------------------------------------------//
    assetArg ( name, qualifier ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_METHOD ));
        this.top ().assetArgs [ name ] = qualifier;
        return this;
    }

    //----------------------------------------------------------------//
    constArg ( name, qualifier ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_METHOD ));
        this.top ().constArgs [ name ] = qualifier;
        return this;
    }

    //----------------------------------------------------------------//
    constraint ( name, description, qualifier ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_METHOD ));
        this.top ().constraints.push ({
            description:    description,
            constraint:     qualifier,
        });
        return this;
    }

    //----------------------------------------------------------------//
    constructor ( name ) {

        this.stack = [];

        this.schema = {
            name:               name,
            lua:                '',
            definitions:        {},
            meta:               '',
            methods:            {},
        };

        this.push (
            SCHEMA_BUILDER_ADDING_SCHEMA,
            this.schema
        );
    }

    //----------------------------------------------------------------//
    definition ( name ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_SCHEMA ));

        this.push (
            SCHEMA_BUILDER_ADDING_ASSET_DEFINITION,
            {
                fields:         {},
            },
            ( schema, definition ) => {
                schema.definitions [ name ] = definition;
            }
        );
        return this;
    }

    //----------------------------------------------------------------//
    done () {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_SCHEMA ));
        return this.schema;
    }

    //----------------------------------------------------------------//
    extends ( base ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE ));
        this.top ().extends = base;
        return this;
    }

    //----------------------------------------------------------------//
    field ( name, value ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_DEFINITION ));

        let field = makeAssetFieldValue ( value );
        field.mutable = false;

        this.push (
            SCHEMA_BUILDER_ADDING_ASSET_DEFINITION_FIELD,
            field,
            ( definition, field ) => {
                definition.fields [ name ] = field;
            }
        );
        return this;
    }

    //----------------------------------------------------------------//
    lua ( lua ) {

        // pop to adding method first
        assert (
            this.popTo ( SCHEMA_BUILDER_ADDING_METHOD ) ||
            this.popTo ( SCHEMA_BUILDER_ADDING_SCHEMA )
        );
        this.top ().lua = lua;
        return this;
    }

    //----------------------------------------------------------------//
    luaFile ( filename ) {

        // pop to adding method first
        assert (
            this.popTo ( SCHEMA_BUILDER_ADDING_METHOD ) ||
            this.popTo ( SCHEMA_BUILDER_ADDING_SCHEMA )
        );

        let lua = fs.readFileSync ( filename, 'utf8' );
        assert ( lua );
        //lua = jsonEscape ( lua );

        this.top ().lua = lua;
        return this;
    }

    //----------------------------------------------------------------//
    meta ( meta ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_SCHEMA ));
        this.top ().meta = meta;
        return this;
    }

    //----------------------------------------------------------------//
    metaFile ( filename ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_SCHEMA ));

        let meta = fs.readFileSync ( filename, 'utf8' );
        assert ( meta );

        this.top ().meta = JSON.parse ( meta );
        return this;
    }

    //----------------------------------------------------------------//
    method ( name, description ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_SCHEMA ));

        this.push (
            SCHEMA_BUILDER_ADDING_METHOD,
            {
                weight:         1,
                maturity:       0,
                description:    description || '',
                assetArgs:      {},
                constArgs:      {},
                constraints:    [],
                lua:            '',
            },
            ( schema, method ) => {
                schema.methods [ name ] = method;
            }
        );
        return this;
    }

    //----------------------------------------------------------------//
    mutable ( base ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_DEFINITION_FIELD ));
        this.top ().mutable = true;
        return this;
    }

    //----------------------------------------------------------------//
    popTo ( state ) {

        let found = false;
        for ( let i in this.stack ) {
            if ( this.stack [ i ].state === state ) {
                found = true;
                break;
            }
        }
        if ( !found ) return false;

        let i = this.stack.length - 1;
        while ( this.stack [ i ].state !== state ) {
            this.stack [ i ].resolve ( this.stack [ i - 1 ].container, this.stack [ i ].container );
            this.stack.pop ();
            i--;
        }
        return true;
    }

    //----------------------------------------------------------------//
    push ( state, container, resolve ) {

        this.stack.push ({
            state:          state,
            container:      container,
            resolve:        resolve,
        });
    }

    //----------------------------------------------------------------//
    string ( base ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE_FIELD ));
        this.top ().type = TYPE_STRING;
        return this;
    }

    //----------------------------------------------------------------//
    top () {
        return this.stack [ this.stack.length - 1 ].container;
    }
}

export const buildSchema = ( name, displayName ) => { return new SchemaBuilder ( name, displayName ); }
