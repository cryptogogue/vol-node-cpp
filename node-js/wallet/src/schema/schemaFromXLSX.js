/* eslint-disable no-whitespace-before-property */

import { buildSchema, op }      from '../schema/SchemaBuilder';
import * as excel               from '../util/excel'
import { JUSTIFY }              from '../util/textLayout';
import * as util                from '../util/util';
import fs                       from 'fs';
import handlebars               from 'handlebars';
import _                        from 'lodash';

const COMPILE_OPTIONS = {
    noEscape: true,
}

//----------------------------------------------------------------//
export function makeSchemaTransaction ( schema, accountName, keyName, gratuity, nonce ) {

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

//----------------------------------------------------------------//
function numberParam ( name, fallback ) {
    return {
        type:       'number',
        name:       name,
        fallback:   fallback,
    }
}

//----------------------------------------------------------------//
function readDefinitions ( schemaBuilder, macros, sheet, name, row ) {

    // read in the field definitions
    const fieldDefs = {};
    for ( let col = 2; col < sheet.width; ++col ) {

        const name = sheet.getValueByCoord ( col, row, false );
        const type = sheet.getValueByCoord ( col, row + 1, false );

        if ( name && type ) {
            fieldDefs [ col ] = {
                name:   name,
                type:   type,
            }
        }
    }

    // skip the field definitions
    row += 2;

    for ( let row = 0; row < sheet.height; ++row ) {

        const cardNumber = parseInt ( sheet.getValueByCoord ( 1, row ));
        if ( !cardNumber ) continue;

        let definition = {};
        let fieldCount = 0;

        for ( let col = 2; col < sheet.width; ++col ) {

            const fieldDef = fieldDefs [ col ];
            if ( !fieldDef ) continue;

            const raw = sheet.getValueByCoord ( col, row, '' );
            let value;

            switch ( fieldDef.type ) {
                case 'number':
                    value = Number ( raw );
                    if ( typeof ( value ) !== fieldDef.type ) continue;
                    break;
                case 'string':
                    value = String ( raw );
                    if ( typeof ( value ) !== fieldDef.type ) continue;
                    value = handlebars.compile ( value, COMPILE_OPTIONS )( macros );
                    break;
                default:
                    continue;
            }

            definition [ fieldDef.name ] = value;
            fieldCount++;
        }

        const name = escapeDefinitionName ( definition.name );

        if ( name.length > 0 ) {

            schemaBuilder.definition ( name );
            for ( let fieldName in definition ) {
                schemaBuilder.field ( fieldName, definition [ fieldName ]);
            }
        }
    }
}

//----------------------------------------------------------------//
function readFonts ( schemaBuilder, sheet, name, row, paramNames ) {

    const params = readParams ( sheet, row++, paramNames, [
        stringParam ( 'name' ),
        stringParam ( 'regular' ),
        stringParam ( 'bold', false ),
        stringParam ( 'italic', false ),
        stringParam ( 'boldItalic', false ),
    ]);

    schemaBuilder.font ( name, params.regular );

    if ( params.bold ) {
        schemaBuilder.bold ( params.bold );
    }

    if ( params.italic ) {
        schemaBuilder.italic ( params.italic );
    }

    if ( params.bolditalic ) {
        schemaBuilder.bolditalic ( params.bolditalic );
    }
}

//----------------------------------------------------------------//
function readIcons ( schemaBuilder, sheet, name, row, paramNames ) {

    const params = readParams ( sheet, row++, paramNames, [
        stringParam ( 'name' ),
        numberParam ( 'width' ),
        numberParam ( 'height' ),
        stringParam ( 'svg' ),
    ]);

    schemaBuilder.icon ( name, params.width, params.height, params.svg );
}

//----------------------------------------------------------------//
function readLayouts ( schemaBuilder, sheet, name, row, paramNames ) {

    const H_JUSTIFY = {
        left:       JUSTIFY.HORIZONTAL.LEFT,
        center:     JUSTIFY.HORIZONTAL.CENTER,
        right:      JUSTIFY.HORIZONTAL.RIGHT,
    };

    const V_JUSTIFY = {
        bottom:     JUSTIFY.VERTICAL.BOTTOM,
        center:     JUSTIFY.VERTICAL.CENTER,
        top:        JUSTIFY.VERTICAL.TOP,
    };

    const params = readParams ( sheet, row++, paramNames, [
        stringParam ( 'name' ),
        stringParam ( 'svg', false ),
        numberParam ( 'width' ),
        numberParam ( 'height' ),
        numberParam ( 'dpi' ),
    ]);

    const docWidth     = params.width;
    const docHeight    = params.height;

    schemaBuilder.layout ( name, docWidth, docHeight, params.dpi, params.svg );

    for ( ; row < sheet.height; ++row ) {

        const draw = util.toStringOrFalse ( sheet.getValueByCoord ( paramNames.draw, row ));
        if ( !draw ) break;

        switch ( draw ) {

            case 'pdf417': {

                const drawParams = readParams ( sheet, row, paramNames, [
                    stringParam ( 'draw' ),
                    stringParam ( 'text' ),
                    stringParam ( 'svg', false ),
                    numberParam ( 'x', 0 ),
                    numberParam ( 'y', 0 ),
                    numberParam ( 'width' ),
                    numberParam ( 'height' ),
                ]);

                schemaBuilder.drawBarcodePDF417 (
                    drawParams.text,
                    drawParams.x,
                    drawParams.y,
                    drawParams.width,
                    drawParams.height
                );

                schemaBuilder.wrapSVG ( drawParams.svg );

                break;
            }

            case 'qr': {

                const drawParams = readParams ( sheet, row, paramNames, [
                    stringParam ( 'draw' ),
                    stringParam ( 'text' ),
                    stringParam ( 'svg', false ),
                    numberParam ( 'x', 0 ),
                    numberParam ( 'y', 0 ),
                    numberParam ( 'width' ),
                    stringParam ( 'qrErr', false ),
                    numberParam ( 'qrType', false ),
                ]);

                schemaBuilder.drawBarcodeQR (
                    drawParams.text,
                    drawParams.x,
                    drawParams.y,
                    drawParams.width,
                    drawParams.qrErr,
                    drawParams.qrType
                );

                schemaBuilder.wrapSVG ( drawParams.svg );

                break;
            }

            case 'ref': {

                const drawParams = readParams ( sheet, row, paramNames, [
                    stringParam ( 'draw' ),
                    stringParam ( 'text' ),
                    stringParam ( 'svg', false ),
                    stringParam ( 'x', 0 ),
                    stringParam ( 'y', 0 ),
                ]);

                schemaBuilder.drawLayout (
                    drawParams.text,
                    drawParams.x,
                    drawParams.y,
                    drawParams.svg
                );

                break;
            }

            case 'svg': {
                
                const drawParams = readParams ( sheet, row, paramNames, [
                    stringParam ( 'draw' ),
                    stringParam ( 'svg' )
                ]);

                schemaBuilder.drawSVG (
                    drawParams.svg,
                );

                break;
            }

            case 'textbox': {

                const drawParams = readParams ( sheet, row, paramNames, [
                    stringParam ( 'draw' ),
                    stringParam ( 'text' ),
                    stringParam ( 'svg', false ),
                    numberParam ( 'x', 0 ),
                    numberParam ( 'y', 0 ),
                    numberParam ( 'width', docWidth ),
                    numberParam ( 'height', docHeight ),
                    stringParam ( 'font' ),
                    numberParam ( 'fontSize' ),
                    stringParam ( 'hAlign', 'left' ),
                    stringParam ( 'vAlign', 'top' ),
                ]);

                schemaBuilder.drawTextBox (
                    drawParams.x,
                    drawParams.y,
                    drawParams.width,
                    drawParams.height,
                    V_JUSTIFY [ drawParams.vAlign ]
                );

                schemaBuilder.wrapSVG ( drawParams.svg );

                schemaBuilder.drawText (
                    drawParams.text,
                    drawParams.font,
                    drawParams.fontSize,
                    H_JUSTIFY [ drawParams.hAlign ]
                );

                break;
            }

            case '+text': {

                const drawParams = readParams ( sheet, row, paramNames, [
                    stringParam ( 'draw' ),
                    stringParam ( 'text' ),
                    stringParam ( 'font' ),
                    numberParam ( 'fontSize' ),
                    stringParam ( 'hAlign', 'left' ),
                ]);

                schemaBuilder.drawText (
                    drawParams.text,
                    drawParams.font,
                    drawParams.fontSize,
                    H_JUSTIFY [ drawParams.hAlign ]
                );

                break;
            }
        }
    }
}

//----------------------------------------------------------------//
function readParamNames ( sheet, row ) {
    const paramNames = {};
    for ( let col = 1; col < sheet.width; ++col ) {
        const name = util.toStringOrFalse ( sheet.getValueByCoord ( col, row ));
        if ( name ) {
            paramNames [ name ] = col;
        }
    }
    return paramNames;
}

//----------------------------------------------------------------//
function readParams ( sheet, row, paramNames, paramDecls ) {

    paramDecls = paramDecls || [];

    const params = {};
    const validNames = {};

    for ( let decl of paramDecls ) {

        const name = decl.name;
        validNames [ name ] = true;

        let val = sheet.getValueByCoord ( paramNames [ name ], row );

        if ( val === undefined ) {
            val = decl.fallback;
        }
        else {
            switch ( decl.type ) {
                case 'number':
                    val = Number ( val );
                    break;
                case 'string':
                    val = String ( val );
                    break;
            }
        }

        if ( val === undefined ) {
            console.log ( `ROW ${ row + 1 } ERROR: param ${ name } is required.` );
        }
        else {
            params [ name ] = val;
        }
    }

    for ( const name in paramNames ) {
        const val = sheet.getValueByCoord ( paramNames [ name ], row );
        if (( val !== undefined ) && ( _.has ( validNames, name ) === false )) {
            console.log ( `ROW ${ row + 1 } WARNING: param ${ name }:${ val } will be ignored.` );
        }
    }

    return params;
}

//----------------------------------------------------------------//
function readMacros ( macros, sheet, name, row, paramNames ) {
    const params = readParams ( sheet, row++, paramNames, [
        stringParam ( 'name' ),
        stringParam ( 'value', '' ),
    ]);
    const val = String ( params.value || '' );
    macros [ name ] = val;
}

//----------------------------------------------------------------//
function stringParam ( name, fallback ) {
    return {
        type:       'string',
        name:       name,
        fallback:   fallback,
    }
}

//----------------------------------------------------------------//
export function writeJavascriptToFile ( schema, filename ) {

    const out = `export const ${ schema.name } = JSON.parse ( \`${ JSON.stringify ( schema ).replace ( /(\\)/g, `\\\\` )}\` );`;
    fs.writeFileSync ( filename, out, 'utf8' );
}

//----------------------------------------------------------------//
export function writeJSONToFile ( schema, filename ) {

    fs.writeFileSync ( filename, JSON.stringify ( schema, null, 4 ), 'utf8' );
}

//----------------------------------------------------------------//
export function writeTransactionToFile ( schema, filename ) {

    fs.writeFileSync ( filename, JSON.stringify ( makeSchemaTransaction ( schema, '9090' ), null, 4 ), 'utf8' );
}

//================================================================//
// util
//================================================================//

//----------------------------------------------------------------//
function escapeDefinitionName ( name ) {

    name = name || '';
    return name.replace ( /\s+/g, '-' ).toLowerCase ();
}

//----------------------------------------------------------------//
export function schemaFromXLSX ( book ) {

    const schemaBuilder = buildSchema ( 'TEST_SCHEMA' );
    const macros = {};

    const readSheet = ( sheetName ) => {

        const sheet = book.getSheet ( sheetName );
        if ( !sheet ) return;

        let handlers = {
            FONTS:          ( name, row, paramNames ) => { readFonts        ( schemaBuilder, sheet, name, row, paramNames )},
            ICONS:          ( name, row, paramNames ) => { readIcons        ( schemaBuilder, sheet, name, row, paramNames )},
            LAYOUTS:        ( name, row, paramNames ) => { readLayouts      ( schemaBuilder, sheet, name, row, paramNames )},
            MACROS:         ( name, row, paramNames ) => { readMacros       ( macros, sheet, name, row, paramNames )},
        }

        let mode = false;
        let paramNames = false;

        for ( let row = 0; row < sheet.height; ++row ) {

            const nextMode = util.toStringOrFalse ( sheet.getValueByCoord ( 0, row ));

            if ( nextMode === 'DEFINITIONS' ) {
                readDefinitions  ( schemaBuilder, macros, sheet, name, row );
            }
            else {
                if ( nextMode ) {
                    mode = nextMode;
                    paramNames = readParamNames ( sheet, row );
                }
                else if ( paramNames ) {
                    const name = util.toStringOrFalse ( sheet.getValueByCoord ( paramNames.name, row ));
                    if ( name ) {
                        handlers [ mode ]( name, row, paramNames );
                    }
                }
            }
        }
    }

    readSheet ( 'layouts' );
    readSheet ( 0 );

    return schemaBuilder.done ();
}
