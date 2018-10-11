/* eslint-disable no-whitespace-before-property */

// TODO: this doesn't seem cryptographically secure...

//----------------------------------------------------------------//
//Use the browser's crypto module to generate random bytes
export const randomBytes = ( size, cb ) => {

    let Buffer = require ( 'safe-buffer' ).Buffer;
    let crypto = global.crypto || global.msCrypto;
    
    if ( size > 65536 ) throw new Error ( 'requested too many random bytes' );
    
    // in case browserify isn't using the Uint8Array version
    let rawBytes = new global.Uint8Array ( size );

    // This will not work in older browsers.
    // See https://developer.mozilla.org/en-US/docs/Web/API/window.crypto.getRandomValues
    if ( size > 0 ) {  // getRandomValues fails on IE if size == 0
        crypto.getRandomValues ( rawBytes );
    }

    // Pass rawBytes to Buffer
    let bytes = Buffer.from ( rawBytes.buffer );

    if ( typeof cb === 'function' ) {
        return process.nextTick ( function () {
            cb ( null, bytes );
        })
    }

    return bytes;
}

// TODO: Implement browser check in app
// if (crypto && crypto.getRandomValues) {
//     export default randomBytes;
// } else {
//     export default oldBrowser;
// }

// export const oldBrowser = () => {
//     throw new Error('Secure random number generation is not supported by this browser.\nUse Chrome, Firefox or Internet Explorer 11');
// }