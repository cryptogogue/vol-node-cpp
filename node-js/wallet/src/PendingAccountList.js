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
import * as UI                              from 'semantic-ui-react';

//================================================================//
// PendingAccountView
//================================================================//
const PendingAccountView = observer (( props ) => {

    const { appState, pending } = props;

    const createAccountRequest = () => {
        appState.deleteAccountRequest ( pending.requestID );
    }

    return (
        <UI.Form size = "large" onSubmit = {() => { createAccountRequest ()}}>
            <UI.Segment stacked>
                <UI.Header as="h2" color="teal" textAlign="center">Account Request</UI.Header>
                <UI.Segment style = {{ wordWrap: 'break-word' }}>
                    { pending.encoded }
                </UI.Segment>
                <UI.Button color = "red" fluid size = "large">
                    Delete
                </UI.Button>
            </UI.Segment>
        </UI.Form>
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
        <UI.Form size = "large" onSubmit = {() => { importAccount ()}}>
            <UI.Segment>
                <h3>{ pending.accountID }</h3>
                <UI.Form.Input
                    fluid
                    icon = "lock"
                    iconPosition = "left"
                    placeholder = "Wallet Password"
                    type = "password"
                    value = { password }
                    onChange = {( event ) => { setPassword ( event.target.value )}}
                />
                <UI.Button color = "teal" fluid size = "large" disabled = { !submitEnabled }>
                    Import
                </UI.Button>
            </UI.Segment>
        </UI.Form>
    );
});

//================================================================//
// PendingAccountList
//================================================================//
export const PendingAccountList = observer (( props ) => {

    const { appState } = props;

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
        <UI.List>
            { requests }
        </UI.List>
    );
});
