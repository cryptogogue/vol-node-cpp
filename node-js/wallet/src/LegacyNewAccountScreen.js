/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import { AppStateService }                  from './AppStateService';
import { NavigationBar }                    from './NavigationBar';
import { Service, useService }              from './Service';
import * as util                            from './util/util';
import { action, computed, extendObservable, observable, observe } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment } from 'semantic-ui-react';

import * as crypto                  from './util/crypto';

const STATUS_SEARCHING_FOR_BIDS         = 0;
const STATUS_NO_PROVIDERS_FOUND         = 1;
const STATUS_NO_BIDS_FOUND              = 2;
const STATUS_FOUND_BID                  = 3;
const STATUS_POSTING_TRANSACTION        = 4;
const STATUS_DONE                       = 5;

//================================================================//
// NewAccountScreenController
//================================================================//
class NewAccountScreenController extends Service {
    
    //----------------------------------------------------------------//
    async acceptBid () {

        const bid = this.bestBid;
        if ( !bid ) return;

        console.log ( 'ACCEPTING BID' );

        this.setState ({ status: STATUS_POSTING_TRANSACTION })

        const order = {
            accountName:    this.accountID,
            keyName:        'master',
            publicKey:      this.publicKey,
            amount:         0,
        };

        try {

            let data = await this.revocableFetchJSON ( bid.provider + '/bid', {
                method : 'POST',
                headers : { 'content-type': 'application/json' },
                body : JSON.stringify ( order )
            });

            console.log ( 'GOT TRANSACTION' );
            console.log ( data );
            this.postTransaction ( data );
        }
        catch ( error ) {

            // TODO: handle error
            console.log ( error );
        }
    }

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        this.appState = appState;

        // Create random string for account ID
        const accountID = 'vol_' + Math.random ().toString ( 36 ).substr ( 2, 9 );

        const mnemonic = crypto.generateMnemonic ();
        const key = crypto.mnemonicToKey ( mnemonic );
        const privateKey = key.getPrivateHex ();
        const publicKey = key.getPublicHex ();

        extendObservable ( this, {
            accountID:      accountID,
            privateKey:     privateKey,
            publicKey:      publicKey,
            seedPhrase:     mnemonic,
            status:         STATUS_SEARCHING_FOR_BIDS,
            searchCount:    0,
            providers:      [],
            bestBid:        false,
        });

        this.miners = [];

        this.revocableTimeout (() => { this.searchForBids ()}, 0 );
    }

    //----------------------------------------------------------------//
    async postTransaction ( transaction ) {

        const { minerURLs } = this.props.appState;

        console.log ( 'POSTING TRANSACTION' );
        console.log ( 'MINERS', minerURLs );

        this.status = STATUS_POSTING_TRANSACTION;

        let success = 0;

        let postTransaction = async ( url ) => {
            
            console.log ( 'POST TO MINER:', url );

            try {
                await this.revocableFetch ( url + '/transactions', {
                    method : 'POST',
                    headers : { 'content-type': 'application/json' },
                    body : JSON.stringify ( transaction )
                });

                success++;
            }
            catch ( error ) {
                console.log ( error );
            }
        }

        let promises = [];
        minerURLs.forEach (( url ) => {
            promises.push ( postTransaction ( url ));
        });
        await this.revocableAll ( promises );

        if ( success > 0 ) {
            this.this.appState.saveAccount ( this.accountID, this.privateKey, this.publicKey );
            this.revocableSetState ({ status: STATUS_DONE });
        }
    }

    //----------------------------------------------------------------//
    async searchForBids () {

        const { marketURLs } = this.appState;

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

//----------------------------------------------------------------//
function renderBid ( service ) {

    const { status, bestBid } = service.state;

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
function renderButton ( service ) {

    const isEnabled = ( service.state.status === STATUS_FOUND_BID );

    return (
        <Button primary icon labelPosition = "right" disabled = { !isEnabled } onClick = {() => { service.acceptBid ()}}>
            Accept bid
            <Icon name = "right arrow" />
        </Button>
    );
}

//================================================================//
// NewAccountScreen
//================================================================//
export const NewAccountScreen = observer (( props ) => {

    const appState      = useService (() => new AppStateService ( util.getUserId ( props )));
    const controller    = useService (() => new NewAccountScreenController ( appState ));

    if ( controller.status === STATUS_DONE ) return appState.redirect ( '/accounts/' + controller.accountID );

    return (
        <div>
            <Header>{ controller.accountID }</Header>
            <Header>Mnemonic seed phrase</Header>
            <p>{ controller.seedPhrase }</p>
            <Header>Keys</Header>
            <p>Public Key: { controller.publicKey }</p>
            <p>Private Key: { controller.privateKey }</p>
            <Header size = "small">You will not be able to recover your seed phrase and private key later</Header>

            { renderBid ()}
            { renderButton ()}
        </div>
    );
});
