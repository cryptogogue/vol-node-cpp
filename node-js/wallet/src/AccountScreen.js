/* eslint-disable no-whitespace-before-property */

import { AppStateService }                  from './AppStateService';
import { NewTransactionModal }              from './NewTransactionModal';
import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';
import { action, computed, extendObservable, observable, observe } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState, useRef }          from 'react';
import { Redirect }                         from 'react-router';
import { useParams }                        from 'react-router-dom';
import * as UI                              from 'semantic-ui-react';

import { AccountNavigationBar, ACCOUNT_TABS } from './AccountNavigationBar';
import { NodeListView }                     from './NodeListView';
import { PendingTransactionsView }          from './PendingTransactionsView';

import { AccountInfoService }               from './AccountInfoService';
import { NodeInfoService }                  from './NodeInfoService';

//================================================================//
// AccountDetailsView
//================================================================//
const AccountDetailsView = observer (( props ) => {

    const { appState }  = props;
    const account       = appState.account;

    if ( !account ) return;

    // const key           = appState.key;
    // const publicKey     = key.publicKeyHex;

    const balance       = appState.balance;
    const textColor     = balance > 0 ? 'black' : 'red';

    // const keyEntitlements = key.entitlements ? JSON.stringify ( key.entitlements.policy, null, 4 ) : 'entootlements';

    /*
    <If condition = { keyEntitlements }>
        <UI.Modal
            style = {{ height:'auto' }}
            size = "small"
            trigger = {
                <UI.Header.Subheader>
                    { publicKey && publicKey.substr ( 0, 30 ) + "..." }
                </UI.Header.Subheader>
            }
        >
            <UI.Modal.Content>
                <center>
                    <h3>Public Key</h3>
                    <UI.Divider/>
                    <p>{ publicKey }</p>
                    <p>{ keyEntitlements }</p>
                </center>
            </UI.Modal.Content>
        </UI.Modal>
    </If>
    */

    return (
        <div style = {{ textAlign: 'center' }}>
            <UI.Header as = "h2" icon>
                <Choose>
                    <When condition = { appState.hasAccountInfo }>
                        <UI.Icon name = 'trophy' circular />
                    </When>

                    <Otherwise>
                        <UI.Icon name = 'circle notched' loading circular/>
                    </Otherwise>
                </Choose>
                { appState.accountID }
            </UI.Header>

            <div style = {{ display: appState.hasAccountInfo ? 'visible' : 'hidden' }}>
                <UI.Header as = "h2">
                    <p style = {{ color: textColor }}>Balance: { balance }</p>
                    <UI.Header.Subheader>
                        <p>Nonce: { appState.nonce }</p>
                    </UI.Header.Subheader>
                </UI.Header>
            </div>
        </div>
    );
});

//================================================================//
// AccountActionsSegment
//================================================================//
export const AccountActionsSegment = observer (( props ) => {

    const { appState } = props;

    const segmentRef = useRef ();
    const [ transactionModalOpen, setTransactionModalOpen ] = useState ( false );

    return (
        <div ref = { segmentRef }>

            <UI.Popup
                open = {( appState.account.promptFirstTransaction && !transactionModalOpen ) ? true : false }
                content = 'Create and submit transactions.'
                position = 'bottom center'
                context = { segmentRef }
            />

            <UI.Segment>
                <UI.Button
                    fluid
                    color = 'teal'
                    attached = 'bottom'
                    onClick = {() => { setTransactionModalOpen ( true )}}
                >
                    <UI.Icon name = 'envelope'/>
                    New Transaction
                </UI.Button>
            </UI.Segment>

            <NewTransactionModal
                appState = { appState }
                open = { transactionModalOpen }
                onClose = {() => { setTransactionModalOpen ( false )}}
            />
        </div>
    );
});

//================================================================//
// AccountScreen
//================================================================//
export const AccountScreen = observer (( props ) => {

    const networkID = util.getMatch ( props, 'networkID' );
    const accountID = util.getMatch ( props, 'accountID' );

    const appState              = hooks.useFinalizable (() => new AppStateService ( networkID, accountID ));
    const accountInfoService    = hooks.useFinalizable (() => new AccountInfoService ( appState ));

    return (
        <SingleColumnContainerView>

            <AccountNavigationBar
                appState    = { appState }
                tab         = { ACCOUNT_TABS.ACCOUNT }
            />

            <If condition = { appState.hasAccount }>

                <UI.Segment>
                    <AccountDetailsView appState = { appState }/>
                </UI.Segment>

                <If condition = { appState.pendingTransactions.length > 0 }>
                    <UI.Segment>
                        <PendingTransactionsView appState = { appState }/>
                    </UI.Segment>
                </If>

                <AccountActionsSegment appState = { appState }/>
            </If>

        </SingleColumnContainerView>
    );
});
