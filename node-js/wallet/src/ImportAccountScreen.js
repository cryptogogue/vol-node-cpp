/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import { AppStateService }                  from './AppStateService';
import { NavigationBar }                    from './NavigationBar';
import { Service, useService }              from './Service';
import { SingleColumnContainerView }        from './SingleColumnContainerView'
import * as util                            from './util/util';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment } from 'semantic-ui-react';

// https://www.npmjs.com/package/js-crypto-utils

import * as crypto              from './util/crypto';

const STATUS_WAITING_FOR_INPUT          = 0;
const STATUS_VERIFYING_KEY              = 1;
const STATUS_DONE                       = 2;

//================================================================//
// ImportAccountScreenController
//================================================================//
class ImportAccountScreenController extends Service {

    @observable accountID       = '';
    @observable errorMessage    = '';
    @observable phraseOrKey     = '';
    @observable keyError        = false;
    @observable status          = STATUS_WAITING_FOR_INPUT;
    @observable password        = '';

    //----------------------------------------------------------------//
    checkPassword () {
    
        return this.appState.checkPassword ( this.password );
    }

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();
        this.appState   = appState;
        this.key        = false;
    }

    //----------------------------------------------------------------//
    @action
    handleChange ( event ) {

        this [ event.target.name ] = event.target.value;
    }

    //----------------------------------------------------------------//
    hasValidKey () {

        return (( this.phraseOrKey.length > 0 ) && ( !this.keyError ));
    }

    //----------------------------------------------------------------//
    @action
    async import () {

        const appState = this.appState;
        const key = this.key;

        const publicKey = key.getPublicHex ();
        console.log ( 'PUBLIC_KEY', publicKey );

        // check to see if we already have the key, in which case early out
        let accountID = appState.findAccountIdByPublicKey ( publicKey );

        if ( accountID ) {

            console.log ( 'ACCOUNT KEY ALREADY EXISTS' );

            this.accountID = accountID;
            this.status = STATUS_DONE;

            return;
        }

        const keyID = key.getKeyID ();
        console.log ( 'KEY_ID', keyID );

        this.status = STATUS_VERIFYING_KEY;

        let keyName = false;

        try {
            const data = await this.revocableFetchJSON ( appState.node + '/keys/' + keyID );

            const keyInfo = data && data.keyInfo;

            if ( keyInfo ) {
                accountID = keyInfo.accountName;
                keyName = keyInfo.keyName;
            }
        }
        catch ( error ) {
            console.log ( error );
        }

        runInAction (() => {
            if ( accountID ) {

                const privateKey = key.getPrivateHex ();
                appState.affirmAccountAndKey (
                    this.password,
                    accountID,
                    keyName,
                    this.phraseOrKey,
                    privateKey,
                    publicKey
                );

                this.accountID = accountID;
                this.status = STATUS_DONE;
            }
            else {
                this.errorMessage = 'Account not found.';
                this.status = STATUS_WAITING_FOR_INPUT;
            }
        });
    }

    //----------------------------------------------------------------//
    @action
    setPassword ( password ) {

        this.password = password;
    }

    //----------------------------------------------------------------//
    @action
    async setPhraseOrKey ( phraseOrKey ) {

        this.phraseOrKey = phraseOrKey;
        this.keyError = false;
        this.key = false;

        try {
            const key = await crypto.loadKeyAsync ( this.phraseOrKey );
            
            console.log ( 'KEY_ID',         key.getKeyID ());
            console.log ( 'PUBLIC_KEY',     key.getPublicHex ());
            console.log ( 'PRIVATE_KEY',    key.getPrivateHex ());

            runInAction (() => { this.key = key });
        }
        catch ( error ) {
            runInAction (() => { this.keyError = true });
        }
    }
}

//================================================================//
// ImportAccountScreen
//================================================================//
export const ImportAccountScreen = observer (( props ) => {

    const appState      = useService (() => new AppStateService ( util.getUserId ( props )));
    const controller    = useService (() => new ImportAccountScreenController ( appState ));

    if ( !appState.hasUser ()) return appState.redirect ( '/' );
    if ( !appState.isLoggedIn ()) return appState.redirect ( '/login' );

    const hasMiners         = appState.node.length > 0;
    const inputEnabled      = hasMiners;
    const submitEnabled     = inputEnabled && controller.checkPassword () && controller.hasValidKey ();

    if ( controller.status === STATUS_DONE ) return appState.redirect ( '/accounts/' + controller.accountID );

    let warning;
    if ( !appState.node.length > 0 ) {
        warning = (
            <Header as="h4" color="red" textAlign="center">
                No mining node specified.
            </Header>
        );
    }

    return (
        <SingleColumnContainerView>
        
            <NavigationBar navTitle = "Import Account" appState = { appState }/>

            { warning }
            <Form size = "large" onSubmit = {() => { controller.import ()}}>
                <Segment stacked>
                    <Form.Input
                        fluid
                        icon = "lock"
                        iconPosition = "left"
                        placeholder = "Wallet Password"
                        type = "password"
                        value = { controller.password }
                        onChange = {( event ) => { controller.setPassword ( event.target.value )}}
                    />
                    <div className = "ui hidden divider" ></div>
                    <Form.TextArea
                        placeholder = "Mnemonic Phrase or Private Key"
                        rows = { 8 }
                        name = "phraseOrKey"
                        value = { controller.phraseOrKey }
                        onChange = {( event ) => { controller.setPhraseOrKey ( event.target.value )}}
                        error = { controller.keyError }
                        disabled = { !inputEnabled }
                    />
                    {( controller.keyError > 0 ) && <span>{ 'Invalid Key Type' }</span>}
                    <Button color = "teal" fluid size = "large" disabled = { !submitEnabled }>
                        Import
                    </Button>
                </Segment>
            </Form>

        </SingleColumnContainerView>
    );
});
