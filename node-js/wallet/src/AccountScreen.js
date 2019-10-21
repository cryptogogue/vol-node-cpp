/* eslint-disable no-whitespace-before-property */

import { AppStateService }                  from './AppStateService';
import { KeySelector }                      from './KeySelector';
import { Service, useService }              from './Service';
import { SingleColumnContainerView }        from './SingleColumnContainerView'
import * as util                            from './util/util';
import { action, computed, extendObservable, observable, observe } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment } from 'semantic-ui-react';

import { NavigationBar }                    from './NavigationBar';
import { NodeListView }                     from './NodeListView';
import { PendingTransactionsView }          from './PendingTransactionsView';
import { StagedTransactionsView }           from './StagedTransactionsView';

import { AccountInfoService }               from './AccountInfoService';
import { NodeInfoService }                  from './NodeInfoService';

import { TransactionFormSelector }          from './TransactionFormSelector';

//================================================================//
// AccountDetailsView
//================================================================//
const AccountDetailsView = observer (( props ) => {

    const { appState }  = props;
    const account       = appState.account;

    if ( !account ) return;

    const key           = appState.key;
    const publicKey     = key.publicKeyHex;
    const nodes         = appState.nodes;

    const balance       = appState.balance;
    const textColor     = balance > 0 ? 'black' : 'red';

    const keyEntitlements = key.entitlements ? JSON.stringify ( key.entitlements.policy, null, 4 ) : false;

    return (
        <div>
            <Header as = "h2" icon>
                <Icon name = "key" circular />
                { appState.accountID }
            </Header>

            <h4>
                <KeySelector appState = { appState }/>
            </h4>

            <If condition = { keyEntitlements }>
                <Modal style={{ height:'auto' }} size = "small" trigger = { <Header.Subheader>{ publicKey && publicKey.substr ( 0, 30 ) + "..." }</Header.Subheader> }>
                    <Modal.Content>
                        <center>
                            <h3>Public Key</h3>
                            <Divider/>
                            <p>{ keyEntitlements }</p>
                        </center>
                    </Modal.Content>
                </Modal>
            </If>

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

    Object.keys ( accounts ).forEach (( accountID ) => {
        options.push ({ key:accountID, value:accountID, text:accountID });
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
export const AccountScreen = observer (( props ) => {

    const accountIDFromEndpoint = util.getMatch ( props, 'accountID' );

    const appState              = useService (() => new AppStateService ( util.getMatch ( props, 'userID' ), accountIDFromEndpoint ));
    const accountInfoService    = useService (() => new AccountInfoService ( appState ));
    const nodeInfoService       = useService (() => new NodeInfoService ( appState ));

    const userID        = appState.userID;
    const accountID     = appState.accountID;

    // TODO: move redirects to a HOC
    if ( !appState.hasUser ()) return appState.redirect ( '/' );
    if ( !appState.isLoggedIn ()) return appState.redirect ( '/login' );

    console.log ( 'APPSTATE ACCOUNT ID:', accountID );

    if ( accountID !== accountIDFromEndpoint ) {
        return appState.redirect ( '/accounts/' + accountID );
    }

    let userName;
    if ( userID.length > 0 ) {
        userName = (<Header as = 'h2'>{ userID }</Header>);
    }

    return (
        <SingleColumnContainerView>
            <div style = {{ 
                backgroundColor :   'LightSlateGray',
                color           :   'white',
                height          :   '20px',
                left            :   '0',
                position        :   'fixed',
                top             :   '0',
                width           :   '100%',
                zIndex          :   '1000'
            }}>
                Network: { appState.nodeInfo.network || 'UNKNOWN' }
            </div>

            <NavigationBar navTitle = "Accounts" appState = { appState }/>

            <If condition = { Object.keys ( appState.accounts ).length > 1 }>
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

        </SingleColumnContainerView>
    );
});
