/* eslint-disable no-whitespace-before-property */

import fs                       from 'fs';
import { buildSchema, op }      from '../wallet/src/Schema/SchemaBuilder.js';
import XLSX                     from 'xlsx';

const aaaCache = [];

//----------------------------------------------------------------//
export function aaaToIndex ( aaa ) {

    const base = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';

    let result = 0;
    for ( let i = 0, j = aaa.length - 1; i < aaa.length; ++i, --j ) {
        result += Math.pow ( base.length, j ) * ( base.indexOf ( aaa [ i ]) + 1 );
    }
    return result - 1;
}

//----------------------------------------------------------------//
export function addrToCoord ( addr ) {

    // this is lazy...
    let col = addr.replace ( /([^a-z])/ig, '' );
    let row = addr.replace ( /([^0-9])/ig, '' );

    col = aaaToIndex ( col );
    row = parseInt ( row ) - 1;

    return [ col, row ];
}

//----------------------------------------------------------------//
export function coordToAddr ( col, row ) {

    col = indexToAAA ( col );
    row = ( row || 0 ) + 1;
    return `${ col }${ row }`;
}

//----------------------------------------------------------------//
export function indexToAAA ( index ) {

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

//================================================================//
// Worksheet
//================================================================//
export class Worksheet {

    //----------------------------------------------------------------//
    constructor ( workbook, sheet ) {

        this.workbook   = workbook;
        this.sheet      = sheet;

        const [ width, height ] = this.getExtents ();

        this.width = width;
        this.height = height;
    }
    
    //----------------------------------------------------------------//
    getExtents () {

        const ref = this.sheet [ '!ref' ];
        const maxAddr = ref.split ( ':' )[ 1 ];
        const [ col, row ] = addrToCoord ( maxAddr );
        return [ col + 1, row + 1 ];
    }

    //----------------------------------------------------------------//
    getValueByCoord ( col, row, fallback ) {

        const cell = this.sheet [ coordToAddr ( col, row )];
        return cell ? cell.v : fallback;
    }
}

//================================================================//
// Workbook
//================================================================//
export class Workbook {

    //----------------------------------------------------------------//
    constructor ( filename ) {

        this.book           = XLSX.readFile ( filename );
        this.sheets         = this.book.Sheets;
        this.sheetNames     = this.book.SheetNames;
    }

    //----------------------------------------------------------------//
    getSheet ( sheetname ) {

        sheetname = ( typeof ( sheetname ) === 'string' ) ? sheetname : this.sheetNames [ sheetname ]
        return new Worksheet ( this, this.sheets [ sheetname ]);
    }
}

