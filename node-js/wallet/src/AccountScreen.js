/* eslint-disable no-whitespace-before-property */

import { AppStateService }                  from './AppStateService';
import { KeySelector }                      from './KeySelector';
import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';
import { action, computed, extendObservable, observable, observe } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Redirect }                         from 'react-router';
import { useParams }                        from 'react-router-dom';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment } from 'semantic-ui-react';

import { AccountNavigationBar, ACCOUNT_TABS } from './AccountNavigationBar';
import { NodeListView }                     from './NodeListView';
import { PendingTransactionsView }          from './PendingTransactionsView';

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

    const balance       = appState.balance;
    const textColor     = balance > 0 ? 'black' : 'red';

    const keyEntitlements = key.entitlements ? JSON.stringify ( key.entitlements.policy, null, 4 ) : 'entootlements';

    return (
        <div style = {{ textAlign: 'center' }}>
            <Header as = "h2" icon>
                <Icon name = "trophy" circular />
                { appState.accountID }
            </Header>

            <h4>
                <KeySelector appState = { appState }/>
            </h4>

            <If condition = { keyEntitlements }>
                <Modal
                    style = {{ height:'auto' }}
                    size = "small"
                    trigger = {
                        <Header.Subheader>
                            { publicKey && publicKey.substr ( 0, 30 ) + "..." }
                        </Header.Subheader>
                    }
                >
                    <Modal.Content>
                        <center>
                            <h3>Public Key</h3>
                            <Divider/>
                            <p>{ publicKey }</p>
                            <p>{ keyEntitlements }</p>
                        </center>
                    </Modal.Content>
                </Modal>
            </If>

            <Choose>

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
// AccountScreen
//================================================================//
export const AccountScreen = observer (( props ) => {

    const networkID = util.getMatch ( props, 'networkID' );
    const accountID = util.getMatch ( props, 'accountID' );

    const appState              = hooks.useFinalizable (() => new AppStateService ( networkID, accountID ));
    const accountInfoService    = hooks.useFinalizable (() => new AccountInfoService ( appState ));

    // // TODO: move redirects to a HOC
    // if ( !appState.hasUser ()) return (<Redirect to = { '/' }/>);
    // if ( !appState.isLoggedIn ()) return (<Redirect to = { '/login' }/>);

    return (
        <SingleColumnContainerView>

            <AccountNavigationBar
                appState    = { appState }
                tab         = { ACCOUNT_TABS.ACCOUNT }
            />

            <If condition = { appState.hasAccount }>

                <Segment>
                    <AccountDetailsView appState = { appState }/>
                </Segment>

                <If condition = { appState.pendingTransactions.length > 0 }>
                    <Segment>
                        <PendingTransactionsView appState = { appState }/>
                    </Segment>
                </If>

                <Segment>
                    <TransactionFormSelector appState = { appState }/>
                </Segment>
            </If>

        </SingleColumnContainerView>
    );
});
