/* eslint-disable no-whitespace-before-property */

//================================================================//
// color
//================================================================//

//----------------------------------------------------------------//
export function copy ( c ) {
    return c ? make ( c.r, c.g, c.b, c.a ) : false;
}

//----------------------------------------------------------------//
export function fromHex ( hex ) {

    const color = make ();

    hex = hex.charAt ( 0 ) == '#' ? hex.slice ( 1 ) : hex;

    const component = ( i ) => {
        i = i * 2;
        const n = parseInt ( hex.slice ( i, i + 2 ), 16 ) / 255;
        return Math.round ( n * 100 ) / 100
    }

    if ( hex.length >= 6 ) {
        color.r = component ( 0 );
        color.g = component ( 1 );
        color.b = component ( 2 );
    }

    if ( hex.length == 8 ) {
        color.a = component ( 3 );
    }

    return color;
}

//----------------------------------------------------------------//
export function make ( r, g, b, a ) {

    return {
        r:      r || 0,
        g:      g || 0,
        b:      b || 0,
        a:      a || 1,
    };
}

//----------------------------------------------------------------//
export function toHexRGB ( c ) {

    const r = Number ( c.r * 255 ).toString ( 16 ).padStart ( 2, '0' );
    const g = Number ( c.g * 255 ).toString ( 16 ).padStart ( 2, '0' );
    const b = Number ( c.b * 255 ).toString ( 16 ).padStart ( 2, '0' );

    return `#${r}${g}${b}`;
}
