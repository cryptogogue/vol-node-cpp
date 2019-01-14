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
const IS                = makeFuncOp    ( 'IS' );
const LESS_OR_EQUAL     = makeBinaryOp  ( 'LESS_OR_EQUAL' );
const LESS_THAN         = makeBinaryOp  ( 'LESS' );
const MOD               = makeBinaryOp  ( 'MOD' );
const MUL               = makeBinaryOp  ( 'MUL' );
const NOT               = makeUnaryOp   ( 'NOT' );
const NOT_EQUAL         = makeBinaryOp  ( 'NOT_EQUAL' );
const OR                = makeBinaryOp  ( 'OR' );
const SUB               = makeBinaryOp  ( 'SUB' );
const XOR               = makeBinaryOp  ( 'XOR' );

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

    classes: {

        pack: {
            displayName:    'Booster Pack',
        },
        
        common: {
            displayName:    'Common',
            keywords:       [ 'card', 'common' ],
        },

        rare: {
            displayName:    'Rare',
            keywords:       [ 'card', 'rare' ],
        },

        ultraRare: {
            displayName:    'Ultra-Rare',
            keywords:       [ 'card', 'ulraRare' ],
        },
    },

    rules: {
 
        openPack: {
            description:    'Open a booster pack.',
            qualifiers:     IS ( 'pack' ),
        },

        makeRare: {
            description:    'Combine two commons to make a rare.',
            qualifiers:     [
                IS ( 'common' ),
                IS ( 'common' ),
            ],
        },

        makeUltraRare: {
            description:    'Combine two rares to make an ultra-rare.',
            qualifiers:     [
                IS ( 'rare' ),
                IS ( 'rare' ),
            ],
        },
    },
}

transaction = makeSchemaTransaction ( schema );

fs.writeFileSync ( 'schema.json', JSON.stringify ( transaction, null, 4 ), 'utf8' );
