/* eslint-disable no-whitespace-before-property */

import * as excel               from './excel'
import fs                       from 'fs';
import handlebars               from 'handlebars';

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
