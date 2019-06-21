/* eslint-disable no-whitespace-before-property */

const fs                = require ( 'fs' );
const buildSchema       = require ( 'volition-schema-builder' ).buildSchema;

const op = buildSchema.op;

//================================================================//
// DO IT
//================================================================//

//----------------------------------------------------------------//
function makeSchemaTransaction ( schema, accountName, keyName, gratuity, nonce ) {

    return {
        type:       'PUBLISH_SCHEMA',
        maker: {
            accountName:    accountName,
            keyName:        keyName || 'master',
            gratuity:       gratuity || 0,
            nonce:          nonce || 0,
        },
        name:       schema.name,
        schema:     schema,
    }
}

//================================================================//
// schema
//================================================================//
let schema = buildSchema ( 'TEST_SCHEMA' )
    .metaFile ( 'meta/meta.json' )
    .luaFile ( 'lua/publish.lua' )

    //----------------------------------------------------------------//
    .definition ( 'pack' )
 
    .definition ( 'common' )
        .field ( 'keywords', 'card common' )
 
    .definition ( 'rare' )
        .field ( 'keywords', 'card rare' )
 
    .definition ( 'ultraRare' )
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
fs.writeFileSync ( 'publish-schema-transaction.json', JSON.stringify ( makeSchemaTransaction ( schema, '9090' ), null, 4 ), 'utf8' );
