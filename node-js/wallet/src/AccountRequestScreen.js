/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import { AccountRequestService }            from './AccountRequestService';
import { AppStateService }                  from './AppStateService';
import { NetworkNavigationBar, NETWORK_TABS } from './NetworkNavigationBar';
import { assert, crypto, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';
import _                                    from 'lodash';
import { action, computed, extendObservable, observable, observe }              from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Redirect }                         from 'react-router';
import { Button, Divider, Dropdown, Form, Header, Icon, Modal, Segment }  from 'semantic-ui-react';

//================================================================//
// AccountReqestForm
//================================================================//
const AccountReqestForm = observer (( props ) => {

    const { appState } = props;

    if ( !appState.hasUser ()) return (<Redirect to = { '/' }/>);
    if ( !appState.isLoggedIn ()) return (<Redirect to = { '/login' }/>);

    const [ password, setPassword ]         = useState ( '' );
    const [ phraseOrKey, setPhraseOrKey ]   = useState ( false );
    const [ keyError, setKeyError ]         = useState ( false );
    const [ key, setKey ]                   = useState ( false );

    const onChange = async ( phraseOrKey ) => {

        console.log ( 'SET PHRASE OR KEY' );
        console.log ( phraseOrKey );
        
        setKey ( false );
        setPhraseOrKey ( phraseOrKey );

        try {
            const key = await crypto.loadKeyAsync ( phraseOrKey );
            setKey ( key );
            setKeyError ( false );
            console.log ( 'KEY IS OKAY' );
        }
        catch ( error ) {
            setKeyError ( true );
            console.log ( 'KEY IS NOT OKAY' );
        }
    }

    const createAccountRequest = () => {
        appState.setAccountRequest (
            password,
            phraseOrKey,
            key.getKeyID (),
            key.getPrivateHex (),
            key.getPublicHex ()
        );
        setPassword ( '' );
        setPhraseOrKey ( false );
        setKey ( false );
    }

    if ( !phraseOrKey ) {
        onChange ( crypto.generateMnemonic ());
    }

    const submitEnabled = key && appState.checkPassword ( password );

    return (
        <div>
            <Form size = "large" onSubmit = {() => { createAccountRequest ()}}>
                <Segment stacked>
                    <Form.TextArea
                        placeholder = "Mnemonic Phrase or Private Key"
                        rows = { 8 }
                        name = "phraseOrKey"
                        value = { phraseOrKey }
                        onChange = {( event ) => { onChange ( event.target.value )}}
                        error = { keyError }
                    />
                    <Form.Input
                        fluid
                        icon = "lock"
                        iconPosition = "left"
                        placeholder = "Wallet Password"
                        type = "password"
                        value = { password }
                        onChange = {( event ) => { setPassword ( event.target.value )}}
                    />
                    <Button color = "teal" fluid size = "large" disabled = { !submitEnabled }>
                        Create Account Request
                    </Button>
                </Segment>
            </Form>
        </div>
    );
});

//================================================================//
// PendingAccountView
//================================================================//
const PendingAccountView = observer (( props ) => {

    const { appState, pending } = props;

    const createAccountRequest = () => {
        appState.deleteAccountRequest ( pending.requestID );
    }

    return (
        <div>
            <Form size = "large" onSubmit = {() => { createAccountRequest ()}}>
                <Segment stacked>
                    <Header as="h2" color="teal" textAlign="center">Account Request</Header>
                    <Segment stacked style = {{ wordWrap: 'break-word' }}>
                        { pending.encoded }
                    </Segment>
                    <Button color = "red" fluid size = "large">
                        Delete
                    </Button>
                </Segment>
            </Form>
        </div>
    );
});

//================================================================//
// ImportAccountView
//================================================================//
const ImportAccountView = observer (( props ) => {

    const { appState, pending } = props;
    const [ password, setPassword ] = useState ( '' );

    const importAccount = () => {
        appState.importAccountRequest ( pending.requestID, password );
    }

    const submitEnabled = appState.checkPassword ( password );

    return (
        <div>
            <Form size = "large" onSubmit = {() => { importAccount ()}}>
                <Segment stacked>
                    <h3>{ pending.accountID }</h3>
                    <Form.Input
                        fluid
                        icon = "lock"
                        iconPosition = "left"
                        placeholder = "Wallet Password"
                        type = "password"
                        value = { password }
                        onChange = {( event ) => { setPassword ( event.target.value )}}
                    />
                    <Button color = "teal" fluid size = "large" disabled = { !submitEnabled }>
                        Import
                    </Button>
                </Segment>
            </Form>
        </div>
    );
});

//================================================================//
// AccountRequestScreen
//================================================================//
export const AccountRequestScreen = observer (( props ) => {

    const networkIDFromEndpoint     = util.getMatch ( props, 'networkID' );
    const appState                  = hooks.useFinalizable (() => new AppStateService ());
    const accountRequestService     = hooks.useFinalizable (() => new AccountRequestService ( appState ));

    const pending = _.values ( appState.pendingAccounts )[ 0 ] || false;

    let requests = [];
    for ( let requestID in appState.pendingAccounts ) {
        const pending = appState.pendingAccounts [ requestID ];
        
        if ( pending.readyToImport ) {
            requests.push (<ImportAccountView key = { requestID } appState = { appState } pending = { pending }/>);
        }
        else {
            requests.push (<PendingAccountView key = { requestID } appState = { appState } pending = { pending }/>);
        }
    }

    return (
        <SingleColumnContainerView>
            <NetworkNavigationBar
                appState = { appState }
                tab = { NETWORK_TABS.ACCOUNT_REQUESTS }
                networkID = { networkIDFromEndpoint }
            />
            <AccountReqestForm appState = { appState }/>
            { requests }
        </SingleColumnContainerView>
    );
});
