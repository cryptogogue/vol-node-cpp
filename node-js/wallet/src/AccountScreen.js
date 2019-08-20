/* eslint-disable no-whitespace-before-property */

import { AppStateService }                                                      from './AppStateService';
import { Service, useService }                                                  from './Service';
import * as util                                                                from './util/util';
import { action, computed, extendObservable, observable, observe }              from 'mobx';
import { observer }                                                             from 'mobx-react';
import React, { useState }                                                      from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment }  from 'semantic-ui-react';

import NavigationBar            from './NavigationBar';
import NodeListView             from './NodeListView';
import PendingTransactionsView  from './PendingTransactionsView';
import StagedTransactionsView   from './StagedTransactionsView';

import { AccountInfoService }   from './AccountInfoService';
import { NodeInfoService }      from './NodeInfoService';

import TransactionFormSelector  from './TransactionFormSelector';

//================================================================//
// AccountDetailsView
//================================================================//
const AccountDetailsView = observer (( props ) => {

    const { appState } = props;
    const account = appState.account;

    if ( !account ) return;

    const publicKey = account.keys.master.publicKey;
    const nodes = appState.nodes;

    const balance = appState.balance;
    const textColor = balance > 0 ? 'black' : 'red';

    return (
        <div>
            <Header as = "h2" icon>

                <Icon name = "key" circular />
                
                { appState.accountId }

                <Modal size = "small" trigger = { <Header.Subheader>{ publicKey && publicKey.substr ( 0, 30 ) + "..." }</Header.Subheader> }>
                    <Modal.Content>
                        <center>
                            <h3>Public Key</h3>
                            <Divider/>
                            <p>{ publicKey }</p>
                        </center>
                    </Modal.Content>
                </Modal>
            </Header>

            <Choose>

                <When condition = { nodes.length === 0 }>
                    <p>No nodes have been defined. Add nodes below to sync account with chain.</p>
                </When>

                <When condition = { appState.hasAccountInfo }>
                    <Header as = "h2">
                        <p style = {{ color: textColor }}>Balance: { balance }</p>
                        <Header.Subheader>
                            <p>Nonce: { appState.nonce }</p>
                        </Header.Subheader>
                    </Header>
                </When>
                
                <Otherwise>
                    <p>Checking balance...</p>
                </Otherwise>
            </Choose>
        </div>
    );
});

//================================================================//
// AccountSelector
//================================================================//
const AccountSelector = observer (( props ) => {

    const { appState } = props;
    const accounts = appState.accounts;
    let options = [];

    Object.keys ( accounts ).forEach (( accountId ) => {
        options.push ({ key:accountId, value:accountId, text:accountId });
    });

    return (
        <Dropdown 
            placeholder = "Select Account"
            fluid
            search
            selection
            options = { options }
            onChange = {( event, data ) => {
                appState.setAccount ( data.value );
            }}
        />
    );
});

//================================================================//
// AccountScreen
//================================================================//
const AccountScreen = observer (( props ) => {

    const accountIdFromEndpoint = util.getMatch ( props, 'accountId' );

    const appState              = useService (() => new AppStateService ( util.getMatch ( props, 'userId' ), accountIdFromEndpoint ));
    const accountInfoService    = useService (() => new AccountInfoService ( appState ));
    const nodeInfoService       = useService (() => new NodeInfoService ( appState ));

    const userId        = appState.userId;
    const accountId     = appState.accountId;

    // TODO: move redirects to a HOC
    if ( !appState.hasUser ()) return appState.redirect ( '/' );
    if ( !appState.isLoggedIn ()) return appState.redirect ( '/login' );

    console.log ( 'APPSTATE ACCOUNT ID:', accountId );

    if ( accountId !== accountIdFromEndpoint ) {
        return appState.redirect ( '/accounts/' + accountId );
    }

    let userName;
    if ( userId.length > 0 ) {
        userName = (<Header as = 'h2'>{ userId }</Header>);
    }

    return (
        <div>
            <Grid textAlign = "center" style = {{ height: '100%' }} verticalAlign = "middle">
                <Grid.Column style = {{ maxWidth: 450 }}>

                    <NavigationBar navTitle = "Accounts" appState = { appState }/>

                    <If condition = { appState.accounts.length > 1 }>
                        <AccountSelector appState = { appState }/>
                    </If>

                    <If condition = { appState.hasAccount }>

                        <Segment>
                            <AccountDetailsView appState = { appState }/>
                        </Segment>

                        <If condition = { appState.stagedTransactions.length > 0 }>
                            <Segment>
                                <StagedTransactionsView appState = { appState }/>
                            </Segment>
                        </If>

                        <If condition = { appState.pendingTransactions.length > 0 }>
                            <Segment>
                                <PendingTransactionsView appState = { appState }/>
                            </Segment>
                        </If>

                        <Segment>
                            <TransactionFormSelector appState = { appState }/>
                        </Segment>
                    </If>

                    <Segment>
                        <NodeListView appState = { appState }/>
                    </Segment>

                </Grid.Column>
            </Grid>
        </div>
    );
});

export default AccountScreen;
