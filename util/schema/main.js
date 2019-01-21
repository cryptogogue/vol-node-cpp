/* eslint-disable no-whitespace-before-property */

const fs        = require ( 'fs' );

// const CONTENT_AUDIO     = 'audio';
// const CONTENT_IMAGE     = 'image';
// const CONTENT_TEXT      = 'text';
// const CONTENT_VIDEO     = 'video';

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
const CONST             = makeConstOp   ( 'CONST' );
const DIV               = makeBinaryOp  ( 'DIV' );
const EQUAL             = makeBinaryOp  ( 'EQUAL' );
const FIELD             = makeFuncOp    ( 'FIELD' );
const GREATER_THAN      = makeBinaryOp  ( 'GREATER' );
const GREATER_OR_EQUAL  = makeBinaryOp  ( 'GREATER_OR_EQUAL' );
const IN                = makeFuncOp    ( 'IN' );
const IS_ASSET          = makeFuncOp    ( 'IS_ASSET' );
const LESS_OR_EQUAL     = makeBinaryOp  ( 'LESS_OR_EQUAL' );
const LESS_THAN         = makeBinaryOp  ( 'LESS' );
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

const TEMPLATE = {}

//----------------------------------------------------------------//
TEMPLATE.NUMERIC_FIELD = function ( array, mutable ) {

        return {
            type:           'NUMERIC',
            array:          array ? true : false,
            mutable:        mutable ? true : false,
        }
}

//----------------------------------------------------------------//
TEMPLATE.STRING_FIELD = function ( array, mutable ) {

        return {
            type:           'STRING',
            array:          array ? true : false,
            mutable:        mutable ? true : false,
        }
}

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

//================================================================//
// schema
//================================================================//
let schema = {

    name: 'TEST_SCHEMA',

    lua: 'schema.lua',

    assetTemplates: {

        base: {
            fields: {
                displayName:    TEMPLATE.STRING_FIELD (),
            },
        },

        card: {
            extends: 'base',
            fields: {
                keywords:       TEMPLATE.STRING_FIELD ( true ),
            },
        },
    },


    assetDefinitions: {
        
        pack: {
            implements: 'base',
            fields: {
                displayName:    'Booster Pack',
            },
        },

        common: {
            implements: 'base',
            fields: {
                displayName:    'Common',
                keywords:       [ 'card', 'common' ],
            },
        },

        rare: {
            implements: 'base',
            fields: {
                displayName:    'Rare',
                keywords:       [ 'card', 'rare' ],
            },
        },

        ultraRare: {
            base: 'base',
            implements: {
                displayName:    'Ultra-Rare',
                keywords:       [ 'card', 'ulraRare' ],
            },
        },
    },

    methods: {
 
        openPack: {
            description:    'Open a booster pack.',
            weight:         1,
            maturity:       2,
            args: {
                pack:       IS_ASSET ( 'pack' ),
            }
        },

        makeRare: {
            description:    'Combine two commons to make a rare.',
            weight:         1,
            maturity:       2,
            args:     {
                common0:    IS_ASSET ( 'common' ),
                common1:    IS_ASSET ( 'common' ),
            },
        },

        makeUltraRare: {
            description:    'Combine two rares to make an ultra-rare.',
            weight:         1,
            maturity:       2,
            args:     {
                rare0:      IS_ASSET ( 'rare0' ),
                rare1:      IS_ASSET ( 'rare1' ),
            },
        },
    },
}

transaction = makeSchemaTransaction ( schema );

fs.writeFileSync ( 'schema.json', JSON.stringify ( schema, null, 4 ), 'utf8' );
fs.writeFileSync ( 'publish-schema-transaction.json', JSON.stringify ( transaction, null, 4 ), 'utf8' );
