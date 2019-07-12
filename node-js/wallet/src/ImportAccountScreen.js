/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import { AppStateStore }                                                            from './stores/AppStateStore';
import { Service }                                                                  from './stores/Service';
import { Store, useStore }                                                          from './stores/Store';
import * as util                                                                    from './utils/util';
import { action, computed, extendObservable, observable, observe, runInAction }     from 'mobx';
import { observer }                                                                 from 'mobx-react';
import React, { useState }                                                          from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment }      from 'semantic-ui-react';

// https://www.npmjs.com/package/js-crypto-utils

import * as crypto              from './utils/crypto';

const STATUS_WAITING_FOR_INPUT          = 0;
const STATUS_VERIFYING_KEY              = 1;
const STATUS_DONE                       = 2;

//================================================================//
// ImportAccountService
//================================================================//
class ImportAccountService extends Service {

    @observable accountID       = '';
    @observable errorMessage    = '';
    @observable phraseOrKey     = '';
    @observable status          = STATUS_WAITING_FOR_INPUT;

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();
        this.appState = appState;
    }

    //----------------------------------------------------------------//
    @action
    handleChange ( event ) {

        this [ event.target.name ] = event.target.value;
    }

    //----------------------------------------------------------------//
    @action
    async verifyKey ( key ) {

        const appState = this.appState;

        const publicKey = key.getPublicHex ();
        console.log ( 'PUBLIC_KEY', publicKey );

        // check to see if we already have the key, in which case early out
        let accountId = appState.findAccountIdByPublicKey ( publicKey );

        if ( accountId ) {

            console.log ( 'ACCOUNT KEY ALREADY EXISTS' );

            this.accountId = accountId;
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
                accountId = keyInfo.accountName;
                keyName = keyInfo.keyName;
            }
        }
        catch ( error ) {
            console.log ( error );
        }

        runInAction (() => {
            if ( accountId ) {

                const privateKey = key.getPrivateHex ();
                appState.affirmAccountAndKey ( accountId, keyName, privateKey, publicKey );

                this.accountId = accountId;
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
    async verifyPhraseOrKey () {

        try {
            const key = await crypto.loadKeyAsync ( this.phraseOrKey );
            this.verifyKey ( key );
        }
        catch ( error ) {
            console.log ( error );
        }
    }
}

//================================================================//
// ImportAccountScreen
//================================================================//
const ImportAccountScreen = observer (( props ) => {

    const appState      = useStore (() => new AppStateStore ( util.getUserId ( props )));
    const service       = useStore (() => new ImportAccountService ( appState ));

    let onChange    = ( event ) => { service.handleChange ( event )};
    let onSubmit    = () => { service.verifyPhraseOrKey ()};

    const hasMiners         = appState.node.length > 0;
    const inputEnabled      = hasMiners;
    const submitEnabled     = inputEnabled && ( service.phraseOrKey.length > 0 );

    if ( service.status === STATUS_DONE ) return appState.redirect ( '/accounts/' + service.accountId );

    let warning;
    if ( !appState.node.length > 0 ) {
        warning = (
            <Header as="h4" color="red" textAlign="center">
                No mining node specified.
            </Header>
        );
    }

    return (
    
        <div className='login-form'>
            {/*
                The styles below are necessary for the correct render of this form.
                You can do same with CSS, the main idea is that all the elements up to the `Grid`
                below must have a height of 100%.
            */}
            <style>{`
                body > div,
                body > div > div,
                body > div > div > div.login-form {
                    height: 100%;
                }
            `}</style>
            <Grid textAlign = "center" style = {{ height: '100%' }} verticalAlign = "middle">
                <Grid.Column style={{ maxWidth: 450 }}>
                <Header as="h2" color="teal" textAlign="center">
                    Import your account
                </Header>
                { warning }
                <Form size = "large" onSubmit = { onSubmit }>
                    <Segment stacked>
                        <Form.TextArea
                            placeholder = "Mnemonic Phrase or Private Key"
                            name = "phraseOrKey"
                            value = { service.phraseOrKey }
                            onChange = { onChange }
                            error = {( service.errorMessage.length > 0 ) ? true : false }
                            disabled = { !inputEnabled }
                        />
                        {( service.errorMessage.length > 0 ) && <span>{ service.errorMessage }</span>}
                        <Button color = "teal" fluid size = "large" disabled = { !submitEnabled }>
                            Login
                        </Button>
                    </Segment>
                </Form>
                </Grid.Column>
            </Grid>
        </div>
    );
});

export default ImportAccountScreen;
