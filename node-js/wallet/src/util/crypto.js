/* eslint-disable no-whitespace-before-property */

import { randomBytes }          from './randomBytes';
import * as bip32               from 'bip32';
import * as bip39               from 'bip39';
import * as bitcoin             from 'bitcoinjs-lib';
import CryptoJS                 from 'crypto-js';
import keyutils                 from 'js-crypto-key-utils';
import * as secp256k1           from 'secp256k1'

// https://8gwifi.org/ecsignverify.jsp

//================================================================//
// Key
//================================================================//
class Key {

    //----------------------------------------------------------------//
    constructor ( ecpair ) {

        this.ecpair = ecpair;
    }

    //----------------------------------------------------------------//
    getKeyID () {

        // TODO: make this shiz case insensitive!
        return bitcoin.crypto.sha256 ( this.getPublicHex ()).toString ( 'hex' ).toLowerCase ();
    }

    //----------------------------------------------------------------//
    getPrivate () {

        return this.ecpair.privateKey;
    }

    //----------------------------------------------------------------//
    getPrivateHex () {

        return this.getPrivate ().toString ( 'hex' ).toUpperCase ();
    }

    //----------------------------------------------------------------//
    getPublic () {

        return this.ecpair.publicKey;
    }

    //----------------------------------------------------------------//
    getPublicHex () {

        return this.getPublic ().toString ( 'hex' ).toUpperCase ();
    }

    //----------------------------------------------------------------//
    hash ( message ) {

        return bitcoin.crypto.sha256 ( message ).toString ( 'hex' ).toUpperCase ();
    }

    //----------------------------------------------------------------//
    sign ( message ) {

        const signature = this.ecpair.sign ( bitcoin.crypto.sha256 ( message ))
        return secp256k1.signatureExport ( signature ).toString ( 'hex' ).toUpperCase ();
    }

    //----------------------------------------------------------------//
    verify ( message, sigHex ) {

        const signature = Buffer.from ( sigHex, 'hex' );
        return this.ecpair.verify ( bitcoin.crypto.sha256 ( message ), signature );
    }
}

//================================================================//
// api
//================================================================//

//----------------------------------------------------------------//
export function aesCipherToPlain ( ciphertext, password ) {

   return CryptoJS.AES.decrypt ( ciphertext, password ).toString ( CryptoJS.enc.Utf8 );
}

//----------------------------------------------------------------//
export function aesPlainToCipher ( plaintext, password ) {

    return CryptoJS.AES.encrypt ( plaintext, password ).toString ();
}

//----------------------------------------------------------------//
function generateMnemonic ( bytes ) {

    let mnemonic;

    do {
        // Use browser's crypto to get CSPRNG bytes to create mnemonic phrase
        // Broswers before IE 11 and really old Chrome/FF are screwed
        let rb = randomBytes ( bytes || 16 );
        mnemonic = bip39.entropyToMnemonic ( rb.toString ( 'hex' ));
    }
    while ( !bip39.validateMnemonic ( mnemonic ));

    return mnemonic;
}

//----------------------------------------------------------------//
async function loadKeyAsync ( phraseOrPEM ) {

    try {
        if ( bip39.validateMnemonic ( phraseOrPEM )) {
            const key = mnemonicToKey ( phraseOrPEM );
            return key;
        }
    }
    catch ( error ) {

        console.log ( error );
    }
    
    try {
        const key = await pemToKeyAsync ( phraseOrPEM );

        console.log ( key );

        // const publicKey = key.getPublicKeyBuffer ().toString ( 'hex' ).toUpperCase ();
        // console.log ( 'PUBLIC_KEY', publicKey );

        // const privateKey = key.d.toHex ().toUpperCase ();
        // console.log ( 'PRIVATE_KEY', privateKey );

        return key;
    }
    catch ( error ) {
        console.log ( error );
    }

    throw new Error ( 'Unknown key type' );
}

//----------------------------------------------------------------//
function keyFromPrivateHex ( privateKeyHex ) {

    return new Key ( bitcoin.ECPair.fromPrivateKey ( new Buffer ( privateKeyHex, 'hex' )));
}

//----------------------------------------------------------------//
function mnemonicToKey ( mnemonic, path ) {
    
    // ticket basket addict level barrel hobby release ivory luxury sausage modify zero
    // pub: 02E7886533261C4D8201F65C5944DA9FE7940E6B499E590665EA8E21DDB109C7BD
    // priv: B464336E5EA3FC9A322F420E782E352162262831AAEBC7CEEF8BA467CBCA7413
    // addr: 12r9DzsFe13UFTmrafukYqRKSsawgnFPUX

    // https://iancoleman.io/bip39/?#english

    // path = "m/44'/0'/0'/0"
    // path = "m/44'/0'/0'/0/0"

    if ( !bip39.validateMnemonic ( mnemonic )) {
        throw new Error ( 'invalid mnemonic phrase' );
    }

    // Generate seed from mnemonic
    const seed = bip39.mnemonicToSeed ( mnemonic );
    console.log ( 'seed:', seed.toString ( 'hex' ));

    let key = bip32.fromSeed ( seed )

    if ( path ) {
        // Derive BIP32 Extended Keys
        key = key.derivePath ( path );
    }

    console.log ( 'BIP32 xpriv:', key.toBase58 ());
    console.log ( 'BIP32 xpub:', key.neutered ().toBase58 ());

    // const address = key.getAddress ();
    const address = bitcoin.payments.p2pkh ({ pubkey: key.publicKey }).address;
    console.log ( 'address:', address );

    return new Key ( key );
}

//----------------------------------------------------------------//
async function pemToKeyAsync ( pem ) {

    const keyObj = new keyutils.Key ( 'pem', pem );
    const jwk = await keyObj.export ( 'jwk', { outputPublic: true });

    const privKey = new Buffer ( jwk.d, 'base64' );

    return new Key ( bitcoin.ECPair.fromPrivateKey ( privKey ));
}

export { generateMnemonic, keyFromPrivateHex, loadKeyAsync, mnemonicToKey, pemToKeyAsync };
