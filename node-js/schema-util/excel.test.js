/* eslint-disable no-whitespace-before-property */

import fs                       from 'fs';
import { buildSchema, op }      from '../wallet/src/Schema/SchemaBuilder.js';
import XLSX                     from 'xlsx';

//================================================================//
// DO IT
//================================================================//

const aaaCache = [];

//----------------------------------------------------------------//
function aaaToIndex ( aaa ) {

    const base = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';

    let result = 0;
    for ( let i = 0, j = aaa.length - 1; i < aaa.length; ++i, --j ) {
        result += Math.pow ( base.length, j ) * ( base.indexOf ( aaa [ i ]) + 1 );
    }
    return result - 1;
}

//----------------------------------------------------------------//
function indexToAAA ( index ) {

    if ( !aaaCache [ index ]) {

        const baseChar = ( 'A' ).charCodeAt ( 0 );
        let aaa  = '';

        let number = index + 1;

        do {
            number -= 1;
            aaa = String.fromCharCode ( baseChar + ( number % 26 )) + aaa;
            number = ( number / 26 ) >> 0; // quick `floor`
        } while ( number > 0 );

        aaaCache [ index ] = aaa;
    }
    return aaaCache [ index ];
}

//----------------------------------------------------------------//
function addrToCoord ( addr ) {

    // this is lazy...
    let col = addr.replace ( /([^a-z])/ig, '' );
    let row = addr.replace ( /([^0-9])/ig, '' );

    col = aaaToIndex ( col );
    row = parseInt ( row ) - 1;

    return [ col, row ];
}

//----------------------------------------------------------------//
function coordToAddr ( col, row ) {

    col = indexToAAA ( col );
    row = ( row || 0 ) + 1;
    return `${ col }${ row }`;
}

//----------------------------------------------------------------//
function getExtents ( ws ) {

    const ref = ws [ '!ref' ];
    const maxAddr = ref.split ( ':' )[ 1 ];
    const [ col, row ] = addrToCoord ( maxAddr );
    return [ col + 1, row + 1 ];
}

//----------------------------------------------------------------//
function getValueByCoord ( ws, col, row, fallback ) {

    const cell = ws [ coordToAddr ( col, row )];
    return cell ? cell.v : fallback;
}

const wb = XLSX.readFile ( 'cardlist.xlsx' );
const ws = wb.Sheets [ wb.SheetNames [ 0 ]];

const [ MAX_COL, MAX_ROW ] = getExtents ( ws );

for ( let row = 0; row < MAX_ROW; ++row ) {

    const cardNumber = parseInt ( getValueByCoord ( ws, 0, row ));
    if ( !cardNumber ) continue;

    let card = {};

    for ( let col = 0; col < MAX_COL; ++col ) {

        const fieldName = getValueByCoord ( ws, col, 0, false );
        if ( !fieldName ) continue;

        card [ fieldName ] = getValueByCoord ( ws, col, row, '' );
    }

    console.log ( cardNumber, card );
}


