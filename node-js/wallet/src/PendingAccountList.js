/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import { AppStateService }                  from './AppStateService';
import { assert, crypto, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';
import _                                    from 'lodash';
import { action, computed, extendObservable, observable, observe }              from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Redirect }                         from 'react-router';
import * as UI                              from 'semantic-ui-react';

//================================================================//
// AccountRequestService
//================================================================//
export class AccountRequestService {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        
        this.revocable = new RevocableContext ();
        this.checkPendingRequests ( appState, 5000 );
    }

    //----------------------------------------------------------------//
    checkPendingRequests ( appState, delay ) {

        const _fetch = async () => {

            for ( let requestID in appState.pendingAccounts ) {

                const pendingAccount = appState.pendingAccounts [ requestID ];
                if ( pendingAccount.readyToImport ) continue;

                try {

                    const keyID = pendingAccount.keyID;
                    const data = await this.revocable.fetchJSON ( `${ appState.network.nodeURL }/keys/${ keyID }` );

                    const keyInfo = data && data.keyInfo;

                    if ( keyInfo ) {
                        appState.importAccountRequest (
                            requestID,
                            keyInfo.accountName,
                            keyInfo.keyName
                        );
                    }
                }
                catch ( error ) {
                    console.log ( error );
                }
            }

            this.revocable.timeout (() => { this.checkPendingRequests ( appState, delay )}, delay );
        }
        _fetch ();
    }

    //----------------------------------------------------------------//
    finalize () {

        this.revocable.finalize ();
    }
}

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
// PendingAccountList
//================================================================//
export const PendingAccountList = observer (( props ) => {

    const { appState } = props;

    const service = hooks.useFinalizable (() => new AccountRequestService ( appState ));

    let requests = [];
    for ( let requestID in appState.pendingAccounts ) {
        const pending = appState.pendingAccounts [ requestID ];
        requests.push (
            <PendingAccountView
                key = { requestID }
                appState = { appState }
                pending = { pending }
            />
        );
    }

    return (
        <UI.List>
            { requests }
        </UI.List>
    );
});

