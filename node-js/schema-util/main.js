/* eslint-disable no-whitespace-before-property */

import fs                       from 'fs';
import { buildSchema, op }      from '../wallet/src/Schema/SchemaBuilder';
import { JUSTIFY }              from '../wallet/src/util/TextFitter';
import * as util                from './util'

//     //----------------------------------------------------------------//
//     .definition ( 'pack' )
//         .field ( 'layout', 'pack' )
//         .field ( 'displayName', 'Pack' )
//             .alternate ( 'ES', 'El Pack' )
//             .alternate ( 'FR', 'Le Pack' )

//     .definition ( 'common' )
//         .field ( 'layout', 'card' )
//         .field ( 'displayName', 'Common' )
//             .alternate ( 'ES', 'El Common' )
//             .alternate ( 'FR', 'Le Common' )
//         .field ( 'image', 'https://i.imgur.com/VMPKVAN.jpg' )
//         .field ( 'keywords', 'card common' )

//     .definition ( 'rare' )
//         .field ( 'layout', 'card' )
//         .field ( 'displayName', 'Rare' )
//             .alternate ( 'ES', 'El Rare' )
//             .alternate ( 'FR', 'Le Rare' )
//         .field ( 'image', 'https://i.imgur.com/BtKggd4.jpg' )
//         .field ( 'keywords', 'card rare' )

//     .definition ( 'ultraRare' )
//         .field ( 'layout', 'card' )
//         .field ( 'displayName', 'Ultra-Rare' )
//             .alternate ( 'ES', 'El Ultra-Rare' )
//             .alternate ( 'FR', 'Le Ultra-Rare' )
//         .field ( 'image', 'https://i.imgur.com/2aiJ3cq.jpg' )
//         .field ( 'keywords', 'card ultra-rare' )

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

//================================================================//
// schema
//================================================================//
let schemaBuilder = buildSchema ( 'TEST_SCHEMA' )
    .luaFile ( 'lua/publish.lua' )

    //----------------------------------------------------------------//
    .font ( 'roboto', 'http://localhost:3000/fonts/roboto/roboto-regular.ttf' )

    //----------------------------------------------------------------//
    .layout ( 'dude', 750, 1050, 300 )
        .drawSVG (`
            <rect x='0' y='0' width='750' height='1050' fill='#000000'/>
            <rect x='37.5' y='37.5' width='675' height='975' fill='#ffffff'/>
            <rect x='37.5' y='168.75' width='675' height='412.5' fill='#ff0000'/>
            <rect x='37.5' y='900' width='675' height='112.5' fill='#ff0000'/>

            <rect x='48.875' y='37.5' width='534.375' height='56.25' fill='none' stroke='gray'/>
            <rect x='48.875' y='93.75' width='534.375' height='37.5' fill='none' stroke='gray'/>
            <rect x='48.875' y='131.25' width='534.375' height='37.5' fill='none' stroke='gray'/>

            <rect x='48.875' y='592.625' width='652.25' height='296' fill='none' stroke='gray'/>

            <image x='37.5' y='168.75' width='675' height='412.5' xlink:href='{{ image }}'/>
        `)
        // card name
        .drawText ( '{{ name }}', 'roboto', 40, 48.875, 37.5, 534.375, 56.25 )
            .justify ( JUSTIFY.HORIZONTAL.LEFT, JUSTIFY.VERTICAL.CENTER )

        // card type
        .drawText ( '{{ type }}{{ subType }}', 'roboto', 30, 48.875, 93.75, 534.375, 37.5 )
            .justify ( JUSTIFY.HORIZONTAL.LEFT, JUSTIFY.VERTICAL.CENTER )

        // access
        .drawText ( '{{ access }}', 'roboto', 30, 48.875, 131.25, 534.375, 37.5)
            .justify ( JUSTIFY.HORIZONTAL.LEFT, JUSTIFY.VERTICAL.CENTER )

        // rules
        .drawText ( '{{ rules }}', 'roboto', 40, 48.875, 592.625, 652.25, 296 )
            .justify ( JUSTIFY.HORIZONTAL.LEFT, JUSTIFY.VERTICAL.TOP )

        // default barcode is PDF417
        //.drawBarcode ( '{{ $ }}', 37.5, 900, 675, 112.5 )
        
        // same as default, but explicit (better, clearer)
        // .drawBarcodePDF417 ( '{{ $ }}', 37.5, 900, 675, 112.5 )

        // QR code also supported
        // defaults to 'L' and 0 (autoselect type)
        // or specificy one of 'L', 'M', 'Q', 'H' and 0 (autoselect) or types 1-40
        .drawBarcodeQR ( '{{ $ }}', 600, 900, 112.5 )

const schema = util.parseVolitionXLSX ( schemaBuilder );

util.writeJavascript ( schema, 'schema.js' );
util.writeJSON ( schema, 'schema.json' );
util.writeTransaction ( schema, 'publish-schema-transaction.json' );
