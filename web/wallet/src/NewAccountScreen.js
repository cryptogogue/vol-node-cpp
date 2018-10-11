/* eslint-disable no-whitespace-before-property */

import { withAppState }         from './AppStateProvider';
import { randomBytes }          from './utils/randomBytes';
import * as bip39               from 'bip39';
import * as bitcoin             from 'bitcoinjs-lib';
import React, { Component }     from 'react';
import { Redirect }             from 'react-router-dom';
import { Header, Icon, Button } from 'semantic-ui-react';

//================================================================//
// NewAccountScreen
//================================================================//
class NewAccountScreen extends Component {
    
    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        // TODO: posible to show spinner while this sets up?

        // Create random string for account ID
        let accountId = 'vol_' + Math.random ().toString ( 36 ).substr ( 2, 9 );

        // Use browser's crypto to get CSPRNG bytes to create mnemonic phrase
        // Broswers before IE 11 and really old Chrome/FF are screwed
        let rb = randomBytes ( 16 );
        let mnemonic = bip39.entropyToMnemonic ( rb.toString ( 'hex' ));
        
        let privateKey = '';
        let publicKey = '';

        // Check valid mnemonic and create all keys 
        if ( !bip39.validateMnemonic ( mnemonic )) {
            mnemonic = 'Failed to generate phrase.'
        }
        else {

            // Generate seed from mnemonic
            const seed = bip39.mnemonicToSeed ( mnemonic );
            console.log("seed:",seed.toString ( 'hex' ));

            // Create Keypair
            const bitcoinNetwork = bitcoin.networks.bitcoin;
            const hdMaster = bitcoin.HDNode.fromSeedBuffer ( seed, bitcoinNetwork );
            
            // Derive BIP32 Root Key
            const bip32RootKey = hdMaster.toBase58 ();
            console.log ( "BIP32 Root:", bip32RootKey );
            
            // Derive Account Keys
            const AccountExtendedPrivateKey = bitcoin.HDNode.fromBase58 ( bip32RootKey ).derivePath ( "m/44'/0'/0'" ).toBase58 ();
            console.log ( "Account xpriv:", AccountExtendedPrivateKey );
            
            const AccountExtendedPublicKey = bitcoin.HDNode.fromBase58 ( bip32RootKey ).derivePath ( "m/44'/0'/0'" ).neutered ().toBase58 ();
            console.log ( "Account xpub:", AccountExtendedPublicKey )
            
            // Derive BIP32 Extended Keys
            const bip32ExtendedPrivateKey = bitcoin.HDNode.fromBase58 ( bip32RootKey ).derivePath ( "m/44'/0'/0'/0" ).toBase58 ();
            console.log ( "BIP32 xpriv:", bip32ExtendedPrivateKey );
            
            const bip32ExtendedPublicKey = bitcoin.HDNode.fromBase58 ( bip32RootKey ).derivePath ( "m/44'/0'/0'/0" ).neutered ().toBase58 ();
            console.log ( "BIP32 xpub:", bip32ExtendedPublicKey );

            // Derive address from BIP32 xpriv
            const key = bitcoin.HDNode.fromBase58 ( bip32ExtendedPrivateKey ).derivePath ( "0'" );
            const address = key.keyPair.getAddress ();
            console.log ( "address:",address );
            
            // Derive private key
            privateKey = key.keyPair.toWIF ().toUpperCase ();
            
            // Derive public key
            publicKey = key.keyPair.getPublicKeyBuffer ().toString ( 'hex' ).toUpperCase ();
        }

        this.state = {
            accountId: accountId,
            privateKey: privateKey,
            publicKey: publicKey,
            seedPhrase: mnemonic,
            saved: false
        };
    }

    //----------------------------------------------------------------//
    render () {

        if ( this.state.saved ) return (<Redirect to = { '/accounts/' + this.state.accountId }/>);

        return (
            <div>
                <Header>{ this.state.accountId }</Header>
                <Header>Mnemonic seed phrase</Header>
                <p>{ this.state.seedPhrase }</p>
                <Header>Keys</Header>
                <p>Public Key: { this.state.publicKey }</p>
                <p>Private Key: { this.state.privateKey }</p>
                <Header size = "small">You will not be able to recover your seed phrase and private key later</Header>

                <Button primary icon labelPosition = "right" onClick = { this.saveAccount }>
                    Save account
                    <Icon name = "right arrow" />
                </Button>
            </div>
        );
    }

    //----------------------------------------------------------------//
    saveAccount = () => {

        this.setState ({ saved : true });
        this.props.appState.saveAccount ( this.state.accountId, this.state.privateKey, this.state.publicKey );
    }
}

export default withAppState ( NewAccountScreen );
