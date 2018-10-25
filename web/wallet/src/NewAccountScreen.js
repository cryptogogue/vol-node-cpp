/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import { withAppState }             from './AppStateProvider';
import BaseComponent                from './BaseComponent';
import { randomBytes }              from './utils/randomBytes';
import * as bip39                   from 'bip39';
import * as bitcoin                 from 'bitcoinjs-lib';
import React                        from 'react';
import { Redirect }                 from 'react-router-dom';
import { Header, Icon, Button }     from 'semantic-ui-react';

const STATUS_SEARCHING_FOR_BIDS         = 0;
const STATUS_NO_PROVIDERS_FOUND         = 1;
const STATUS_NO_BIDS_FOUND              = 2;
const STATUS_FOUND_BID                  = 3;
const STATUS_POSTING_TRANSACTION        = 4;
const STATUS_DONE                       = 5;

//================================================================//
// NewAccountScreen
//================================================================//
class NewAccountScreen extends BaseComponent {
    
    //----------------------------------------------------------------//
    acceptBid () {

        const bid = this.state.bestBid;
        if ( !bid ) return;

        console.log ( 'ACCEPTING BID' );

        this.setState ({ status: STATUS_POSTING_TRANSACTION })

        const order = {
            accountName:    this.state.accountId,
            keyName:        'master',
            publicKey:      this.state.publicKey,
            amount:         0,
        };

        this.revocablePromise ( fetch ( bid.provider + '/bid', {
            method : 'POST',
            headers : { 'content-type': 'application/json' },
            body : JSON.stringify ( order )
        }))
        .then (( response ) => {

            return response.json ();
        })
        .then (( data ) => {

            console.log ( 'GOT TRANSACTION' );
            console.log ( data );
            this.postTransaction ( data );
        })
        .catch (( error ) => {

            // TODO: handle error
            console.log ( error );
        });
    }

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
            status: STATUS_SEARCHING_FOR_BIDS,
            searchCount: 0,
            providers: [],
            bestBid: false,
        };

        this.miners = [];

        this.revocableTimeout (() => { this.searchForBids (); }, 0 );
    }

    //----------------------------------------------------------------//
    postTransaction ( transaction ) {

        console.log ( 'POSTING TRANSACTION' );
        console.log ( 'MINERS', this.miners );

        this.setState ({ status: STATUS_POSTING_TRANSACTION })

        const nMiners = this.miners.length;
        let count = 0;
        let success = 0;

        let postTransaction = ( url ) => {
            
            console.log ( 'POST TO MINER:', url );

            this.revocablePromise ( fetch ( url + '/transactions', {
                method : 'POST',
                headers : { 'content-type': 'application/json' },
                body : JSON.stringify ( transaction )
            }))
            .then (( response ) => { success++; })
            .catch (( error ) => { console.log ( error ); })
            .finally (() => {
                
                count++;

                if ( count >= nMiners ) {

                    if ( success > 0 ) {
                        this.props.appState.saveAccount ( this.state.accountId, this.state.privateKey, this.state.publicKey );
                        this.setState ({ status: STATUS_DONE });
                    }
                    // TODO: handle failure
                }
            });
        }

        for ( let i in this.miners ) {
            postTransaction ( this.miners [ i ]);
        }
    }

    //----------------------------------------------------------------//
    render () {

        if ( this.state.status === STATUS_DONE ) return (<Redirect to = { '/accounts/' + this.state.accountId }/>);

        return (
            <div>
                <Header>{ this.state.accountId }</Header>
                <Header>Mnemonic seed phrase</Header>
                <p>{ this.state.seedPhrase }</p>
                <Header>Keys</Header>
                <p>Public Key: { this.state.publicKey }</p>
                <p>Private Key: { this.state.privateKey }</p>
                <Header size = "small">You will not be able to recover your seed phrase and private key later</Header>

                { this.renderBid ()}
                { this.renderButton ()}
            </div>
        );
    }

    //----------------------------------------------------------------//
    renderBid () {

        const { status, bestBid } = this.state;

        if ( status === STATUS_SEARCHING_FOR_BIDS ) {
            return (<Header>SEARCHING FOR BIDS...</Header>);
        }
        else if ( status === STATUS_NO_PROVIDERS_FOUND ) {
            return (<Header>NO PROVIDERS FOUND</Header>);
        }
        else if ( status === STATUS_NO_BIDS_FOUND ) {
            return (<Header>NO BIDS FOUND</Header>);
        }
        else if ( bestBid ) {
            return (<Header>{ 'BEST BID: $' + bestBid.accountPrice }</Header>);
        }
    }

    //----------------------------------------------------------------//
    renderButton () {

        const isEnabled = ( this.state.status === STATUS_FOUND_BID );

        return (
            <Button primary icon labelPosition = "right" disabled = { !isEnabled } onClick = {() => { this.acceptBid ()}}>
                Accept bid
                <Icon name = "right arrow" />
            </Button>
        );
    }

    //----------------------------------------------------------------//
    async searchForBids () {

        const { marketURLs } = this.props.appState;

        if ( marketURLs.size <= 0 ) {
            this.setState ({ status: STATUS_NO_PROVIDERS_FOUND });
            return;
        }

        this.setState ({
            status: STATUS_SEARCHING_FOR_BIDS,
            searchCount: 0,
        })

        let searchCount = 0;
        let bestBid = false;

        let checkBid = async ( url ) => {

            try {
                const data = await ( await this.revocableFetch ( url + '/bid')).json ();

                if (( data.type === 'VOL_BID' ) && ( !bestBid  || ( data.volPrice <= bestBid.volPrice ))) {
                    bestBid = data;
                    bestBid.provider = url;
                }
            }
            catch ( error ) {
                console.log ( error );
            }
            this.setState ({ searchCount: searchCount++ });
        }

        let promises = [];
        marketURLs.forEach (( url ) => {
            promises.push ( checkBid ( url ));
        });
        await Promise.all ( promises );
        
        if ( bestBid ) {

            let bid = {
                volPrice:       bestBid.volPrice,
                maxSale:        bestBid.maxSale,
                accountPrice:   bestBid.accountPrice,
                provider:       bestBid.provider,
            };

            this.setState ({
                status: STATUS_FOUND_BID,
                bestBid: bid,
            });
        }
        else {
            this.setState ({ status: STATUS_NO_BIDS_FOUND })
        }
    }
}

export default withAppState ( NewAccountScreen );