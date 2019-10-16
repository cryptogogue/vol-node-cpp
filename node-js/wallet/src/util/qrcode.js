/* eslint-disable no-whitespace-before-property */

import { bitmapToSVG, }         from './bitmapToPaths';
import * as qrcode              from 'qrcode-generator';

export const CONSTS = {
    ID: 'QR',
    ERROR_LEVEL: {
        LOW:        'L',
        MEDIUM:     'M',
        QUARTILE:   'Q',
        HIGH:       'H',
    },
    AUTOSELECT_TYPE: 0,
};

const QR_CAPACITY_ALPHANUMERIC = [];

QR_CAPACITY_ALPHANUMERIC [ 1 ] = [ 25, 20, 16, 10, ];
QR_CAPACITY_ALPHANUMERIC [ 2 ] = [ 47, 38, 29, 20, ];
QR_CAPACITY_ALPHANUMERIC [ 3 ] = [ 77, 61, 47, 35, ];
QR_CAPACITY_ALPHANUMERIC [ 4 ] = [ 114, 90, 67, 50, ];
QR_CAPACITY_ALPHANUMERIC [ 5 ] = [ 154, 122, 87, 64, ];
QR_CAPACITY_ALPHANUMERIC [ 6 ] = [ 195, 154, 108, 84, ];
QR_CAPACITY_ALPHANUMERIC [ 7 ] = [ 224, 178, 125, 93, ];
QR_CAPACITY_ALPHANUMERIC [ 8 ] = [ 279, 221, 157, 122, ];
QR_CAPACITY_ALPHANUMERIC [ 9 ] = [ 335, 262, 189, 143, ];
QR_CAPACITY_ALPHANUMERIC [ 10 ] = [ 395, 311, 221, 174, ];
QR_CAPACITY_ALPHANUMERIC [ 11 ] = [ 468, 366, 259, 200, ];
QR_CAPACITY_ALPHANUMERIC [ 12 ] = [ 535, 419, 296, 227, ];
QR_CAPACITY_ALPHANUMERIC [ 13 ] = [ 619, 483, 352, 259, ];
QR_CAPACITY_ALPHANUMERIC [ 14 ] = [ 667, 528, 376, 283, ];
QR_CAPACITY_ALPHANUMERIC [ 15 ] = [ 758, 600, 426, 321, ];
QR_CAPACITY_ALPHANUMERIC [ 16 ] = [ 854, 656, 470, 365, ];
QR_CAPACITY_ALPHANUMERIC [ 17 ] = [ 938, 734, 531, 408, ];
QR_CAPACITY_ALPHANUMERIC [ 18 ] = [ 1046, 816, 574, 452, ];
QR_CAPACITY_ALPHANUMERIC [ 19 ] = [ 1153, 909, 644, 493, ];
QR_CAPACITY_ALPHANUMERIC [ 20 ] = [ 1249, 970, 702, 557, ];
QR_CAPACITY_ALPHANUMERIC [ 21 ] = [ 1352, 1035, 742, 587, ];
QR_CAPACITY_ALPHANUMERIC [ 22 ] = [ 1460, 1134, 823, 640, ];
QR_CAPACITY_ALPHANUMERIC [ 23 ] = [ 1588, 1248, 890, 672, ];
QR_CAPACITY_ALPHANUMERIC [ 24 ] = [ 1704, 1326, 963, 744, ];
QR_CAPACITY_ALPHANUMERIC [ 25 ] = [ 1853, 1451, 1041, 779, ];
QR_CAPACITY_ALPHANUMERIC [ 26 ] = [ 1990, 1542, 1094, 864, ];
QR_CAPACITY_ALPHANUMERIC [ 27 ] = [ 2132, 1637, 1172, 910, ];
QR_CAPACITY_ALPHANUMERIC [ 28 ] = [ 2223, 1732, 1263, 958, ];
QR_CAPACITY_ALPHANUMERIC [ 29 ] = [ 2369, 1839, 1322, 1016, ];
QR_CAPACITY_ALPHANUMERIC [ 30 ] = [ 2520, 1994, 1429, 1080, ];
QR_CAPACITY_ALPHANUMERIC [ 31 ] = [ 2677, 2113, 1499, 1150, ];
QR_CAPACITY_ALPHANUMERIC [ 32 ] = [ 2840, 2238, 1618, 1226, ];
QR_CAPACITY_ALPHANUMERIC [ 33 ] = [ 3009, 2369, 1700, 1307, ];
QR_CAPACITY_ALPHANUMERIC [ 34 ] = [ 3183, 2506, 1787, 1394, ];
QR_CAPACITY_ALPHANUMERIC [ 35 ] = [ 3351, 2632, 1867, 1431, ];
QR_CAPACITY_ALPHANUMERIC [ 36 ] = [ 3537, 2780, 1966, 1530, ];
QR_CAPACITY_ALPHANUMERIC [ 37 ] = [ 3729, 2894, 2071, 1591, ];
QR_CAPACITY_ALPHANUMERIC [ 38 ] = [ 3927, 3054, 2181, 1658, ];
QR_CAPACITY_ALPHANUMERIC [ 39 ] = [ 4087, 3220, 2298, 1774, ];
QR_CAPACITY_ALPHANUMERIC [ 40 ] = [ 4296, 3391, 2420, 1852, ];

const QR_ERR_TO_INDEX = {
    L:  0,
    M:  1,
    Q:  2,
    H:  3,
}

const QR_LEGAL_CHARS = /^[0-9A-Z$%*+-./ ]*$/;

//================================================================//
// qrcode
//================================================================//

//----------------------------------------------------------------//
export function autoSelectType ( qrErr, size ) {

    const e = QR_ERR_TO_INDEX [ qrErr ];

    for ( let i = 1; i <= 39; ++i ) {
            if ( size <= QR_CAPACITY_ALPHANUMERIC [ i ][ e ] ) return i;
    }
    return 40;
}

//----------------------------------------------------------------//
export function calculateOverflow ( qrType, qrErr, size ) {

    const capacity = getCapacity ( qrType, qrErr );
    return ( size > capacity ) ? size - capacity : 0;
}

//----------------------------------------------------------------//
export function clampType ( qrType ) {

    qrType = qrType < 1 ? 1 : qrType;
    qrType = qrType > 40 ? 40 : qrType;
    return qrType;
}

//----------------------------------------------------------------//
export function getCapacity ( qrType, qrErr ) {

    const i = QR_ERR_TO_INDEX [ qrErr ];
    return QR_CAPACITY_ALPHANUMERIC [ qrType ][ i ];
}

//----------------------------------------------------------------//
export function isLegal ( data ) {

    return QR_LEGAL_CHARS.test ( data );
}

//----------------------------------------------------------------//
export function makeSVGTag ( data, xOff, yOff, width, height, qrErr, qrType ) {

    qrErr = qrErr || 'L';
    qrType = qrType || autoSelectType ( qrErr, data.length );

    const overflow = calculateOverflow ( qrType, qrErr, data.length );
    if ( overflow > 0 ) return false;

    try {
        const qr = qrcode ( qrType, qrErr );
        qr.addData ( data, 'Alphanumeric' );
        qr.make ();

        const moduleCount = qr.getModuleCount ();
        console.log ( 'MODULE COUNT', moduleCount );

        const sampler = ( x, y ) => {
            return qr.isDark ( y, x );
        }
        return bitmapToSVG ( sampler, moduleCount, moduleCount, xOff, yOff, width, height );
    }
    catch ( error ) {
    }
    return false;
}
