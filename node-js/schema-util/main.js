/* eslint-disable no-whitespace-before-property */

const fs                = require ( 'fs' );
const buildSchema       = require ( 'volition-schema-builder' ).buildSchema;

const op = buildSchema.op;

//================================================================//
// DO IT
//================================================================//

//----------------------------------------------------------------//
function makeSchemaTransaction ( schema ) {

    return {
        type:   'PUBLISH_SCHEMA',
        name:   schema.name,
        json:   JSON.stringify ( schema ),
    }
}

//================================================================//
// schema
//================================================================//
let schema = buildSchema ( 'TEST_SCHEMA' )
    .luaFile ( 'lua/publish.lua' )

    //----------------------------------------------------------------//
    .definition ( 'pack' )
        .field ( 'displayName', 'Booster Pack' )
 
    .definition ( 'common' )
        .field ( 'displayName', 'Common' )
        .field ( 'keywords', 'card common' )
 
    .definition ( 'rare' )
        .field ( 'displayName', 'Rare' )
        .field ( 'keywords', 'card rare' )
 
    .definition ( 'ulraRare' )
        .field ( 'displayName', 'Ultra-Rare' )
        .field ( 'keywords', 'card ultra-rare' )

    //----------------------------------------------------------------//
    .method ( 'makeRare', 'Combine two commons to make a rare.' )
        .assetArg ( 'common0', op.ASSET_TYPE ( 'common' ))
        .assetArg ( 'common1', op.ASSET_TYPE ( 'common' ))
        .luaFile ( 'lua/makeRare.lua' )

    .method ( 'makeUltraRare', 'Combine two rares to make an ultra-rare.' )
        .assetArg ( 'rare0', op.ASSET_TYPE ( 'rare' ))
        .assetArg ( 'rare1', op.ASSET_TYPE ( 'rare' ))
        .luaFile ( 'lua/makeUltraRare.lua' )

    .method ( 'openPack', 'Open a booster pack.' )
        .assetArg ( 'pack', op.ASSET_TYPE ( 'pack' ))
        .luaFile ( 'lua/openPack.lua' )

    .done ()

fs.writeFileSync ( 'schema.json', JSON.stringify ( schema, null, 4 ), 'utf8' );
fs.writeFileSync ( 'publish-schema-transaction.json', JSON.stringify ( makeSchemaTransaction ( schema ), null, 4 ), 'utf8' );
