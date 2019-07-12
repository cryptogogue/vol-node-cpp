/* eslint-disable no-whitespace-before-property */

import { AppStateStore }                                                        from './stores/AppStateStore';
import { Service }                                                              from './stores/Service';
import { Store, useStore }                                                      from './stores/Store';
import * as util                                                                from './utils/util';
import { action, computed, extendObservable, observable, observe }              from 'mobx';
import { observer }                                                             from 'mobx-react';
import React, { useState }                                                      from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment }  from 'semantic-ui-react';

import NavigationBar            from './NavigationBar';
import NodeListView             from './NodeListView';

import { AccountInfoService }   from './stores/AccountInfoService';
import { NodeInfoService }      from './stores/NodeInfoService';

import TransactionFormSelector  from './TransactionFormSelector';

//================================================================//
// AccountScreen
//================================================================//
const AccountScreen = observer (( props ) => {

    const accountIdFromEndpoint = util.getAccountId ( props ) || '';

    const appState = useStore (() => new AppStateStore ( util.getUserId ( props ), accountIdFromEndpoint ));
    const accountInfoService = useStore (() => new AccountInfoService ( appState ));
    const nodeInfoService = useStore (() => new NodeInfoService ( appState ));

    const userId = appState.userId;
    const accountId = appState.accountId;

    console.log ( 'ACCOUNT ID:', accountId );
    console.log ( 'URL ACCOUNT ID:', accountIdFromEndpoint );

    // TODO: move redirects to a HOC
    if ( !appState.hasUser ()) return appState.redirect ( '/' );
    if ( !appState.isLoggedIn ()) return appState.redirect ( '/login' );

    console.log ( 'APPSTATE ACCOUNT ID:', accountId );

    if ( accountId !== accountIdFromEndpoint ) {
        console.log ( 'REDIRECT ACCOUNT' );
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

                    <div>
                        { userName }
                        <p>ACTIVE MINERS: { appState.activeMinerCount }</p>
                        <p>ACTIVE MARKETS: { appState.activeMarketCount }</p>
                    </div>

                    <div>
                        { renderAccountSelector ( appState )}
                        { renderAccountDetails ( appState )}
                    </div>

                    <Segment>
                        <TransactionFormSelector
                            appState = { appState }
                            accountId = { accountId }
                            nonce = { appState.nonce }
                        />
                    </Segment>

                    <Segment>
                        <NodeListView appState = { appState }/>
                    </Segment>

                </Grid.Column>
            </Grid>
        </div>
    );
});

//----------------------------------------------------------------//
function renderAccountDetails ( appState ) {

    const account = appState.account;

    if ( !account ) return;

    const publicKey = account.keys.master.publicKey;

    const nodes = appState.nodes;
    let contextAware;
    if ( nodes.length === 0 ) {
        contextAware = (
            <p>No nodes have been defined. Add nodes below to sync account with chain.</p>
        );
    }
    else if ( appState.balance >= 0 ) {
        contextAware = (
            <Header as = "h2">
                <p>Balance: { appState.balance }</p>
                <Header.Subheader>
                    <p>Nonce: { appState.nonce }</p>
                </Header.Subheader>
            </Header>
        );
    }
    else {
        contextAware = (
            <p>Checking balance...</p>
        );
    }

    return (
        <Segment>
            <Header as = "h2" icon>

                <Icon name = "key" circular />
                
                { appState.accountId }

                <Modal size = "small" trigger = { <Header.Subheader>{ publicKey && publicKey.substr ( 0,30 ) + "..." }</Header.Subheader> }>
                    <Modal.Content>
                        <center>
                            <h3>Public Key</h3>
                            <Divider/>
                            <p>{ publicKey }</p>
                        </center>
                    </Modal.Content>
                </Modal>
            </Header>

            { contextAware }

        </Segment>
    );
}

//----------------------------------------------------------------//
function renderAccountSelector ( appState ) {

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
}

//----------------------------------------------------------------//
// function renderTransactions () {

//     const { transactions } = this.props.appState.state;
//     if ( transactions.length === 0 ) return;

//     let count = 0;

//     // transaction type     nonce       pending     rejected        confirmed

//     return (
//         <Segment>
//             { transactions.map (( entry ) => {
//                 return (<p key = { count++ }>{ entry.transaction.friendlyName }</p>);
//             })}
//         </Segment>
//     );
// }

export default AccountScreen;
