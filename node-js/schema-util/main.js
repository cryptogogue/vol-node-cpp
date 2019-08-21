/* eslint-disable no-whitespace-before-property */

import fs                       from 'fs';
import { buildSchema, op }      from '../wallet/src/Schema/SchemaBuilder';
import * as excel               from './excel'

//================================================================//
// DO IT
//================================================================//

//----------------------------------------------------------------//
function escapeName ( name ) {

    name = name || '';
    return name.replace(/\s+/g, '-').toLowerCase ();
}

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
// let schemaBuilder = buildSchema ( 'TEST_SCHEMA' )
//     .luaFile ( 'lua/publish.lua' )

//     //----------------------------------------------------------------//
//     .font ( 'roboto', 'fonts/roboto/roboto-regular.ttf' )

//     //----------------------------------------------------------------//
//     .layout ( 'card', 750, 1050, 300 )
//         .drawSVG (`
//             <rect x="0" y="0" width="750" height="1050" fill="gray" stroke="blue" stroke-width="37.5"/>
//         `)
//         .drawImageField ( 'image', 25, 100, 700, 700 )
//         .drawTextField ( 'displayName', 'roboto', 70, 0, 815, 700, 70 )
//             .justify ( 'CENTER', 'CENTER' )
//             .pen ( 'white' )
//         .drawBarcodeField ( '$', 75, 900, 600, 125 )

//     .layout ( 'pack', 750, 1050, 300 )
//         .drawSVG (`
//             <rect x="0" y="0" width="750" height="1050" fill="gray" stroke="blue" stroke-width="37.5"/>
//             <text x="375" y="560" font-size="150" text-anchor="middle" fill="white">PACK</text>
//         `)
//         .drawTextField ( 'displayName', 'roboto', 70, 0, 815, 700, 70 )
//             .justify ( 'CENTER', 'CENTER' )
//             .pen ( 'white' )
//         .drawBarcodeField ( '$', 75, 900, 600, 125 )

//     //----------------------------------------------------------------//
//     .method ( 'makeRare', 'Combine two commons to make a rare.' )
//         .assetArg ( 'common0', op.ASSET_TYPE ( 'common' ))
//         .assetArg ( 'common1', op.ASSET_TYPE ( 'common' ))
//         .luaFile ( 'lua/makeRare.lua' )

//     .method ( 'makeUltraRare', 'Combine two rares to make an ultra-rare.' )
//         .assetArg ( 'rare0', op.ASSET_TYPE ( 'rare' ))
//         .assetArg ( 'rare1', op.ASSET_TYPE ( 'rare' ))
//         .luaFile ( 'lua/makeUltraRare.lua' )

//     .method ( 'openPack', 'Open a booster pack.' )
//         .assetArg ( 'pack', op.ASSET_TYPE ( 'pack' ))
//         .luaFile ( 'lua/openPack.lua' )
//     ;

let schemaBuilder = buildSchema ( 'TEST_SCHEMA' );

const book = new excel.Workbook ( 'cardlist.xlsx' );
const sheet = book.getSheet ( 0 );

for ( let row = 0; row < sheet.height; ++row ) {

    const cardNumber = parseInt ( sheet.getValueByCoord ( 0, row ));
    if ( !cardNumber ) continue;

    let definition = {};
    let fieldCount = 0;

    for ( let col = 0; col < sheet.width; ++col ) {

        const fieldName = sheet.getValueByCoord ( col, 0, false );
        const fieldType = sheet.getValueByCoord ( col, 1, false );
        if ( !( fieldName && fieldType )) continue;

        const raw = sheet.getValueByCoord ( col, row, '' );
        let value;

        switch ( fieldType ) {
            case 'number':
                value = Number ( raw );
                if ( typeof ( value ) !== fieldType ) continue;
                break;
            case 'string':
                value = String ( raw );
                if ( typeof ( value ) !== fieldType ) continue;
                break;
            default:
                continue;
        }

        definition [ fieldName ] = value;
        fieldCount++;
    }

    const name = escapeName ( definition.name );

    if ( name.length > 0 ) {

        schemaBuilder.definition ( name );
        for ( let fieldName in definition ) {
            schemaBuilder.field ( fieldName, definition [ fieldName ]);
        }
    }
}

const schema = schemaBuilder.done ();

fs.writeFileSync ( 'schema.json', JSON.stringify ( schema, null, 4 ), 'utf8' );
fs.writeFileSync ( 'publish-schema-transaction.json', JSON.stringify ( makeSchemaTransaction ( schema, '9090' ), null, 4 ), 'utf8' );
