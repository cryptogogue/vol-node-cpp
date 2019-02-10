/* eslint-disable no-whitespace-before-property */

const assert    = require ( 'assert' );
const fs        = require ( 'fs' );

//----------------------------------------------------------------//
function makeBinaryOp ( opname ) {
    return ( left, right ) => {
        return {
            op:         opname,
            left:       left,
            right:      right,
        };
    };
}

//----------------------------------------------------------------//
function makeConstOp ( opname ) {
    return ( value ) => {

        let type = typeof ( value );

        return {
            op:         opname,
            type:       type.toUpperCase (),
            value:      value,
        };
    };
}

//----------------------------------------------------------------//
function makeFuncOp ( opname ) {
    return ( ...args ) => {

        let cleanArgs = [];

        args.forEach ( function ( arg ) {

            let argType = typeof ( arg );

            if ( argType != 'object' ) {
                arg = CONST ( arg );
            }
            cleanArgs.push ( arg );
        });

        return {
            op:         opname,
            args:       cleanArgs,
        };
    };
}

//----------------------------------------------------------------//
function makeUnaryOp ( opname ) {
    return ( param ) => {
        return {
            op:         opname,
            param:      param,
        };
    };
}

//----------------------------------------------------------------//
const ADD               = makeBinaryOp  ( 'ADD' );
const AND               = makeBinaryOp  ( 'AND' );
const ASSET_TYPE        = makeFuncOp    ( 'ASSET_TYPE' );
const CONST             = makeConstOp   ( 'CONST' );
const DIV               = makeBinaryOp  ( 'DIV' );
const EQUAL             = makeBinaryOp  ( 'EQUAL' );
const FIELD             = makeFuncOp    ( 'FIELD' );
const GREATER           = makeBinaryOp  ( 'GREATER' );
const GREATER_OR_EQUAL  = makeBinaryOp  ( 'GREATER_OR_EQUAL' );
const IN                = makeFuncOp    ( 'IN' );
const LESS              = makeBinaryOp  ( 'LESS' );
const LESS_OR_EQUAL     = makeBinaryOp  ( 'LESS_OR_EQUAL' );
const MOD               = makeBinaryOp  ( 'MOD' );
const MUL               = makeBinaryOp  ( 'MUL' );
const NOT               = makeUnaryOp   ( 'NOT' );
const NOT_EQUAL         = makeBinaryOp  ( 'NOT_EQUAL' );
const OR                = makeBinaryOp  ( 'OR' );
const SUB               = makeBinaryOp  ( 'SUB' );
const XOR               = makeBinaryOp  ( 'XOR' );

const ARRAY             = 'ARRAY';
const MUTABLE           = 'MUTABLE';
const NUMBER            = 'NUMBER';
const STRING            = 'STRING';

const MEDIA_AUDIO       = 'MEDIA_AUDIO';
const MEDIA_IMAGE       = 'MEDIA_IMAGE';
const MEDIA_TEXT        = 'MEDIA_TEXT';
const MEDIA_VIDEO       = 'MEDIA_VIDEO';

//================================================================//
// DO IT
//================================================================//

//----------------------------------------------------------------//
function jsonEscape ( str ) {
    return str
        .replace ( /\\n/g, "\\n" )
        .replace ( /\\'/g, "\\'" )
        .replace ( /\\"/g, '\\"' )
        .replace ( /\\&/g, "\\&" )
        .replace ( /\\r/g, "\\r" )
        .replace ( /\\t/g, "\\t" )
        .replace ( /\\b/g, "\\b" )
        .replace ( /\\f/g, "\\f" );
};

//----------------------------------------------------------------//
function makeSchemaTransaction ( schema ) {

    let lua = fs.readFileSync ( schema.lua, 'utf8' );
    //lua = jsonEscape ( lua );
    schema.lua = lua;

    return {
        type:   'PUBLISH_SCHEMA',
        name:   schema.name,
        json:   JSON.stringify ( schema ),
    }
}

const SCHEMA_BUILDER_ADDING_SCHEMA                  = 'SCHEMA_BUILDER_ADDING_SCHEMA';
const SCHEMA_BUILDER_ADDING_ASSET_DEFINITION        = 'ADDING_ASSET_DEFINITION';
const SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE          = 'ADDING_ASSET_TEMPLATE';
const SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE_FIELD    = 'ADDING_ASSET_TEMPLATE_FIELD';
const SCHEMA_BUILDER_ADDING_METHOD                  = 'ADDING_ASSET_METHOD';

//================================================================//
// SchemaBuilder
//================================================================//
class SchemaBuilder {

    //----------------------------------------------------------------//
    array ( base ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE_FIELD ));
        this.top ().array = true;
        return this;
    }

    //----------------------------------------------------------------//
    assetArg ( name, qualifier ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_METHOD ));
        this.top ().assetArgs [ name ] = qualifier;
        return this;
    }

    //----------------------------------------------------------------//
    assetDefinition ( name, base ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_SCHEMA ));

        this.push (
            SCHEMA_BUILDER_ADDING_ASSET_DEFINITION,
            {
                extends:    base,
                fields:     {},
            },
            ( schema, assetDefinition ) => {
                schema.assetDefinitions [ name ] = assetDefinition;
            }
        );
        return this;
    }

    //----------------------------------------------------------------//
    assetTemplate ( name ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_SCHEMA ));

        this.push (
            SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE,
            {
                fields:     {},
            },
            ( schema, assetTemplate ) => {
                schema.assetTemplates [ name ] = assetTemplate;
            }
        );
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
    constructor ( name, lua ) {

        this.stack = [];

        this.schema = {
            name:   name,
            lua:    lua,
            assetTemplates:     {},
            assetDefinitions:   {},
            methods:            {},
        };

        this.push (
            SCHEMA_BUILDER_ADDING_SCHEMA,
            this.schema
        );
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

        if ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE )) {

            let field = {
                type:           'STRING',
                array:          false,
                mutable:        false,
            }

            this.push (
                SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE_FIELD,
                field,
                ( assetTemplate, field ) => {
                    assetTemplate.fields [ name ] = field;
                }
            );
            return this;
        }

        if ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_DEFINITION )) {

            this.top ().fields [ name ] = value;
            return this;
        }

        assert ( false );
        return this;
    }

    //----------------------------------------------------------------//
    method ( name, weight, maturity, description ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_SCHEMA ));

        this.push (
            SCHEMA_BUILDER_ADDING_METHOD,
            {
                weight:         weight,
                maturity:       maturity,
                description:    description,
                assetArgs:      {},
                constArgs:      {},
                constraints:    [],
            },
            ( schema, method ) => {
                schema.methods [ name ] = method;
            }
        );
        return this;
    }

    //----------------------------------------------------------------//
    numeric ( base ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE_FIELD ));
        this.top ().type = 'NUMERIC';
        return this;
    }

    //----------------------------------------------------------------//
    mutable ( base ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE_FIELD ));
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
    top () {
        return this.stack [ this.stack.length - 1 ].container;
    }
}


//================================================================//
// schema
//================================================================//
let schema = new SchemaBuilder ( 'TEST_SCHEMA', 'schema.lua' )

    //----------------------------------------------------------------//
    .assetTemplate ( 'base' )
        .field ( 'displayName' )
 
    .assetTemplate ( 'card' ).extends ( 'base' )
        .field ( 'keywords' ).array ()

    //----------------------------------------------------------------//
    .assetDefinition ( 'pack', 'base' )
        .field ( 'displayName', 'Booster Pack' )
 
    .assetDefinition ( 'common', 'card' )
        .field ( 'displayName', 'Common' )
        .field ( 'keywords', [ 'card', 'common' ])
 
    .assetDefinition ( 'rare', 'card' )
        .field ( 'displayName', 'Rare' )
        .field ( 'keywords', [ 'card', 'rare' ])
 
    .assetDefinition ( 'ulraRare', 'card' )
        .field ( 'displayName', 'Ultra-Rare' )
        .field ( 'keywords', [ 'card', 'ultra-rare' ])

    //----------------------------------------------------------------//
    .method ( 'makeRare', 1, 2, 'Combine two commons to make a rare.' )
        .assetArg ( 'common0', ASSET_TYPE ( 'common' ))
        .assetArg ( 'common1', ASSET_TYPE ( 'common' ))

    .method ( 'makeUltraRare', 1, 2, 'Combine two rares to make an ultra-rare.' )
        .assetArg ( 'rare0', ASSET_TYPE ( 'rare' ))
        .assetArg ( 'rare1', ASSET_TYPE ( 'rare' ))

    .method ( 'openPack', 1, 2, 'Open a booster pack.' )
        .assetArg ( 'pack', ASSET_TYPE ( 'pack' ))

    .done ()

fs.writeFileSync ( 'schema.json', JSON.stringify ( schema, null, 4 ), 'utf8' );
//fs.writeFileSync ( 'publish-schema-transaction.json', JSON.stringify ( makeSchemaTransaction ( schema ), null, 4 ), 'utf8' );
