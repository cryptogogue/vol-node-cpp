/* eslint-disable no-whitespace-before-property */

import { randomBytes }          from './randomBytes';
import * as BigInteger          from 'bigi';
import * as bip39               from 'bip39';
import * as bitcoin             from 'bitcoinjs-lib';
import keyutils                 from 'js-crypto-key-utils';
// import * as secp256k1           from 'secp256k1'

//================================================================//
// CryptoKey
//================================================================//
class CryptoKey {

    //----------------------------------------------------------------//
    constructor ( keyPair ) {
        this.keyPair = keyPair;
    }

    //----------------------------------------------------------------//
    getKeyID () {
        return bitcoin.crypto.sha256 ( this.getPublicHex ()).toString ( 'hex' );
    }

    //----------------------------------------------------------------//
    getPrivateHex () {
        return this.keyPair.d.toHex ().toUpperCase ();
    }

    //----------------------------------------------------------------//
    getPublicHex () {
        return this.keyPair.getPublicKeyBuffer ().toString ( 'hex' ).toUpperCase ();
    }
}

//================================================================//
// api
//================================================================//

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
function mnemonicToKey ( mnemonic ) {
    
    if ( !bip39.validateMnemonic ( mnemonic )) {
        throw new Error ( 'invalid mnemonic phrase' );
    }

    // Generate seed from mnemonic
    const seed = bip39.mnemonicToSeed ( mnemonic );
    console.log ( 'seed:', seed.toString ( 'hex' ));

    // Create Keypair
    const bitcoinNetwork = bitcoin.networks.bitcoin;
    const hdMaster = bitcoin.HDNode.fromSeedBuffer ( seed, bitcoinNetwork );
    
    // https://iancoleman.io/bip39/?#english
    
    // Derive BIP32 Extended Keys
    const bip32ExtendedPrivateKey = hdMaster.derivePath ( "m/44'/0'/0'/0" );
    console.log ( 'BIP32 xpriv:', bip32ExtendedPrivateKey.toBase58 ());
    console.log ( 'BIP32 xpub:', bip32ExtendedPrivateKey.neutered ().toBase58 ());

    // Derive address from BIP32 xpriv
    const key = bip32ExtendedPrivateKey.derivePath ( "0" ).keyPair;

    const address = key.getAddress ();
    console.log ( 'address:', address );

    return new CryptoKey ( key );
}

//----------------------------------------------------------------//
async function pemToKeyAsync ( pem ) {

    const keyObj = new keyutils.Key ( 'pem', pem );
    const jwk = await keyObj.export ( 'jwk', { outputPublic: true });

    const privKey = new Buffer ( jwk.d, 'base64' );

    return new CryptoKey ( new bitcoin.ECPair ( BigInteger.fromBuffer ( privKey )));
}

export { generateMnemonic, loadKeyAsync, mnemonicToKey, pemToKeyAsync };
