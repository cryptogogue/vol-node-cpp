/* eslint-disable no-whitespace-before-property */

import * as excel               from '../wallet/src/util/excel'
import { buildSchema, op }      from '../wallet/src/Schema/SchemaBuilder';
import { JUSTIFY }              from '../wallet/src/util/textLayout';
import fs                       from 'fs';
import handlebars               from 'handlebars';
import _                        from 'lodash';

const COMPILE_OPTIONS = {
    noEscape: true,
}

//----------------------------------------------------------------//
function javascriptEscape ( str ) {
    return str
        .replace ( /(\n)/g, `\\n` )
        .replace ( /(\")/g, `\"` )
        .replace ( /(\r)/g, `\\r` )
        .replace ( /(\t)/g, `\\t` )
        .replace ( /(\f)/g, `\\f` );
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
// util
//================================================================//

//----------------------------------------------------------------//
export function caselessCompare ( a, b ) {
    return (( typeof ( a ) === 'string' ) && ( typeof ( b ) === 'string' )) ?
        ( name.localeCompare ( sheetname, undefined, { sensitivity: 'accent' }) === 0 ) :
        ( a === b );
}

//----------------------------------------------------------------//
export function escapeName ( name ) {

    name = name || '';
    return name.replace(/\s+/g, '-').toLowerCase ();
}

//----------------------------------------------------------------//
export function parseVolitionXLSX ( schemaBuilder ) {

    const book = new excel.Workbook ( 'cardlist.xlsx' );

    const macros = {};
    const macrosSheet = book.getSheet ( 'macros' );

    if ( macrosSheet ) {

        for ( let row = 0; row < macrosSheet.height; ++row ) {

            const key = String ( macrosSheet.getValueByCoord ( 0, row ));
            const val = String ( macrosSheet.getValueByCoord ( 1, row ));

            if ( key.length > 0 ) {
                macros [ key ] = val;
            }
        }
    }

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
                    value = handlebars.compile ( value, COMPILE_OPTIONS )( macros );
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

    return schemaBuilder.done ();
}

//----------------------------------------------------------------//
export function parseVolitionXLSX2 ( schemaName ) {

    const book = new excel.Workbook ( 'cardlist.xlsx' );

    const macros = {};
    const layoutSheet = book.getSheet ( 'layouts' );
    if ( !layoutSheet ) return false;

    let schemaBuilder = buildSchema ( schemaName );

    const numberParam = ( name, fallback ) => {
        return {
            type:       'number',
            name:       name,
            fallback:   fallback,
        }
    }

    const stringParam = ( name, fallback ) => {
        return {
            type:       'string',
            name:       name,
            fallback:   fallback,
        }
    }

    const readParamNames = ( row ) => {
        const paramNames = {};
        for ( let col = 1; col < layoutSheet.width; ++col ) {
            const name = toStringOrFalse ( layoutSheet.getValueByCoord ( col, row ));
            if ( name ) {
                paramNames [ name ] = col;
            }
        }
        return paramNames;
    }

    const readParams = ( row, paramNames, paramDecls ) => {

        paramDecls = paramDecls || [];

        const params = {};
        const validNames = {};

        for ( let decl of paramDecls ) {

            const name = decl.name;
            validNames [ name ] = true;

            let val = layoutSheet.getValueByCoord ( paramNames [ name ], row );

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
            const val = layoutSheet.getValueByCoord ( paramNames [ name ], row );
            if (( val !== undefined ) && ( _.has ( validNames, name ) === false )) {
                console.log ( `ROW ${ row + 1 } WARNING: param ${ name }:${ val } will be ignored.` );
            }
        }

        return params;
    }

    const readFonts = ( name, row, paramNames ) => {

        const params = readParams ( row++, paramNames, [
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

    const readIcons = ( name, row, paramNames ) => {

        const params = readParams ( row++, paramNames, [
            stringParam ( 'name' ),
            numberParam ( 'width' ),
            numberParam ( 'height' ),
            stringParam ( 'svg' ),
        ]);

        schemaBuilder.icon ( name, params.width, params.height, params.svg );
    }

    const readLayouts = ( name, row, paramNames ) => {

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

        const params = readParams ( row++, paramNames, [
            stringParam ( 'name' ),
            stringParam ( 'svg', false ),
            numberParam ( 'width' ),
            numberParam ( 'height' ),
            numberParam ( 'dpi' ),
        ]);

        const docWidth     = params.width;
        const docHeight    = params.height;

        schemaBuilder.layout ( name, docWidth, docHeight, params.dpi, params.svg );

        for ( ; row < layoutSheet.height; ++row ) {

            const draw = toStringOrFalse ( layoutSheet.getValueByCoord ( paramNames.draw, row ));
            if ( !draw ) break;

            switch ( draw ) {

                case 'pdf417': {

                    const drawParams = readParams ( row, paramNames, [
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

                    const drawParams = readParams ( row, paramNames, [
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

                    const drawParams = readParams ( row, paramNames, [
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
                    
                    const drawParams = readParams ( row, paramNames, [
                        stringParam ( 'draw' ),
                        stringParam ( 'svg' )
                    ]);

                    schemaBuilder.drawSVG (
                        drawParams.svg,
                    );

                    break;
                }

                case 'textbox': {

                    const drawParams = readParams ( row, paramNames, [
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

                    const drawParams = readParams ( row, paramNames, [
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

    const readMacros = ( name, row, paramNames ) => {
        const params = readParams ( row++, paramNames, [
            stringParam ( 'name' ),
            stringParam ( 'value', '' ),
        ]);
        const val = String ( params.value || '' );
        macros [ name ] = val;
    }

    let handlers = {
        FONTS:      readFonts,
        ICONS:      readIcons,
        LAYOUTS:    readLayouts,
        MACROS:     readMacros,
    }

    let mode = false;
    let paramNames = false;

    for ( let row = 0; row < layoutSheet.height; ++row ) {

        const nextMode = toStringOrFalse ( layoutSheet.getValueByCoord ( 0, row ));
        if ( nextMode ) {
            mode = nextMode;
            paramNames = readParamNames ( row );
        }
        else if ( paramNames ) {
            const name = toStringOrFalse ( layoutSheet.getValueByCoord ( paramNames.name, row ));
            if ( name ) {
                handlers [ mode ]( name, row, paramNames );
            }
        }
    }

    return schemaBuilder;
}

//----------------------------------------------------------------//
export function toNumberOrFalse ( val, types ) {

    types = types || [ 'string', 'number' ];
    return types.includes ( typeof ( val ))  ? Number ( val ) : false;
}

//----------------------------------------------------------------//
export function toStringOrFalse ( val, types ) {

    types = types || [ 'string', 'number' ];
    return types.includes ( typeof ( val ))  ? String ( val ) : false;
}

//----------------------------------------------------------------//
export function writeJavascript ( schema, filename ) {

    const out = `export const ${ schema.name } = JSON.parse ( \`${ JSON.stringify ( schema ).replace ( /(\\)/g, `\\\\` )}\` );`;
    fs.writeFileSync ( filename, out, 'utf8' );
}

//----------------------------------------------------------------//
export function writeJSON ( schema, filename ) {

    fs.writeFileSync ( filename, JSON.stringify ( schema, null, 4 ), 'utf8' );
}

//----------------------------------------------------------------//
export function writeTransaction ( schema, filename ) {

    fs.writeFileSync ( filename, JSON.stringify ( makeSchemaTransaction ( schema, '9090' ), null, 4 ), 'utf8' );
}
