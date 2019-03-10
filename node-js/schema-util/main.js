/* eslint-disable no-whitespace-before-property */

const assert            = require ( 'assert' );
const fs                = require ( 'fs' );
const buildSchema       = require ( 'volition-schema-builder' ).buildSchema;

const op = buildSchema.op;

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
let schema = buildSchema ( 'TEST_SCHEMA', 'schema.lua' )

    //----------------------------------------------------------------//
    .assetTemplate ( 'base' )
        .field ( 'displayName' ).string ()
 
    .assetTemplate ( 'card' ).extends ( 'base' )
        .field ( 'keywords' ).string ().array ()

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
        .assetArg ( 'common0', op.ASSET_TYPE ( 'common' ))
        .assetArg ( 'common1', op.ASSET_TYPE ( 'common' ))

    .method ( 'makeUltraRare', 1, 2, 'Combine two rares to make an ultra-rare.' )
        .assetArg ( 'rare0', op.ASSET_TYPE ( 'rare' ))
        .assetArg ( 'rare1', op.ASSET_TYPE ( 'rare' ))

    .method ( 'openPack', 1, 2, 'Open a booster pack.' )
        .assetArg ( 'pack', op.ASSET_TYPE ( 'pack' ))

    .done ()

fs.writeFileSync ( 'schema.json', JSON.stringify ( schema, null, 4 ), 'utf8' );
fs.writeFileSync ( 'publish-schema-transaction.json', JSON.stringify ( makeSchemaTransaction ( schema ), null, 4 ), 'utf8' );
