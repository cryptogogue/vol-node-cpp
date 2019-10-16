/* eslint-disable no-whitespace-before-property */

import { bitmapToSVG }          from './bitmapToPaths';
import { pdf417 as encoder }    from './pdf417Encoder';

export const CONSTS = {
    ID: 'PDF417',
};

//================================================================//
// qrcode
//================================================================//

//----------------------------------------------------------------//
export function makeSVGTag ( data, xOff, yOff, width, height ) {

    const barcode = encoder ( data );

    const sampler = ( x, y ) => {
        return ( barcode.bcode [ y ][ x ] == 1 );
    }
    return bitmapToSVG ( sampler, barcode.num_cols, barcode.num_rows, xOff, yOff, width, height );
}
