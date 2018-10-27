/* eslint-disable no-whitespace-before-property */

import { withAppStateAndUser }  from './AppStateProvider';
import BaseComponent            from './BaseComponent';
import NavigationBar            from './NavigationBar';
import NodeListView             from './NodeListView';
import TransactionFormSelector  from './TransactionFormSelector';
import React                    from 'react';
import { Dropdown, Segment, Header, Icon, Divider, Modal, Grid } from 'semantic-ui-react';

//================================================================//
// AccountSelection
//================================================================//
class AccountScreen extends BaseComponent {
    
    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        this.state = {
            targetAccountId : this.getAccountId (),
            balance : -1,
            nonce: -1,
        };

        this.getAccountBalance ();
    }

    //----------------------------------------------------------------//
    async getAccountBalance () {

        const accountId = this.getAccountId ();

        let balanceHistogram = new Map ();
        let bestNonce = 0;

        let updateBalance = async ( url ) => {

            try {

                const data = await this.revocableFetchJSON ( url + '/accounts/' + accountId );

                if ( data.account && ( data.account.accountName === accountId )) {

                    let balance = data.account.balance;
                    let nonce = data.account.nonce;

                    let balanceCount = balanceHistogram.has ( balance ) ? balanceHistogram.get ( balance ) : 0;
                    balanceHistogram.set ( balance, balanceCount + 1 );

                    if ( bestNonce < nonce ) {
                        bestNonce = nonce;
                    }
                }
            }
            catch ( error ) {
                console.log ( error );
            }
        }

        if ( accountId ) {

            const { minerURLs } = this.props.appState;

            let promises = [];
            minerURLs.forEach (( url ) => {
                promises.push ( updateBalance ( url ));
            });

            try {

                await this.revocableAll ( promises );

                let bestBalanceCount = 0;
                let bestBalance = -1;

                balanceHistogram.forEach (( balanceCount, balance ) => {
                    if ( bestBalanceCount < balanceCount ) {
                        bestBalance = balance;
                    }
                });

                this.setState ({ balance: bestBalance });
                this.setState ({ nonce: bestNonce });

                this.props.appState.finishTransaction ( accountId, bestNonce );
            }
            catch ( error ) {
                console.log ( error );
            }
        }
        this.revocableTimeout (() => { this.getAccountBalance ()}, 1000 );
    }

    //----------------------------------------------------------------//
    getAccountId () {
        let accountId = this.props.match.params && this.props.match.params.accountId;
        return accountId && ( accountId.length > 0 ) && accountId;
    }

    //----------------------------------------------------------------//
    render () {

        const { appState } = this.props;

        // TODO: move redirects to a HOC
        if ( !appState.hasUser ()) return this.redirect ( '/' );
        if ( !appState.isLoggedIn ()) return this.redirect ( '/login' );

        let targetAccountId = this.state.targetAccountId;
        const accountId = this.getAccountId ();

        if ( !targetAccountId ) {
            const accountNames = Object.keys ( appState.state.accounts );
            targetAccountId = accountNames && accountNames.length && accountNames [ 0 ];
        }

        if ( targetAccountId && ( targetAccountId !== accountId )) {
            return this.redirect ( '/accounts/' + targetAccountId );
        }

        // TODO: nonce should come from account or from last known transaction
        const nonce = this.state.nonce < 0 ? 0 : this.state.nonce;

        let userName;
        if ( appState.state.userId.length > 0 ) {
            userName = (<Header as = 'h2'>{ appState.state.userId }</Header>);
        }

        return (
            <div>
                <Grid textAlign = "center" style = {{ height: '100%' }} verticalAlign = "middle">
                    <Grid.Column style = {{ maxWidth: 450 }}>

                        <NavigationBar navTitle = "Accounts" match = { this.props.match }/>

                        <div>
                            { userName }
                            <p>ACTIVE MINERS: { appState.state.activeMinerCount }</p>
                            <p>ACTIVE MARKETS: { appState.state.activeMarketCount }</p>
                        </div>

                        <div>
                            { this.renderAccountSelector ()}
                            { this.renderAccountDetails ()}
                        </div>

                        <Segment>
                            <TransactionFormSelector
                                accountId = { accountId }
                                nonce = { nonce }
                            />
                        </Segment>

                        <Segment>
                            <NodeListView/>
                        </Segment>

                    </Grid.Column>
                </Grid>
            </div>
        );
    }

    //----------------------------------------------------------------//
    renderAccountDetails () {

        const accountId = this.getAccountId ();
        const { accounts, nodes } = this.props.appState.state;
        const account = ( accountId in accounts ) && accounts [ accountId ];

        if ( !account ) return;

        const publicKey = account.keys.master.publicKey;

        let contextAware;

        if ( nodes.length === 0 ) {
            contextAware = (
                <p>No nodes have been defined. Add nodes below to sync account with chain.</p>
            );
        }
        else if ( this.state.balance >= 0 ) {
            contextAware = (
                <Header as = "h2">
                    <p>Balance: { this.state.balance }</p>
                    <Header.Subheader>
                        <p>Nonce: { this.state.nonce }</p>
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
                    
                    { accountId }

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
    renderAccountSelector () {

        const { accounts } = this.props.appState.state;
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
                    this.setState ({
                        targetAccountId : data.value,
                        balance : -1,
                        nonce: -1
                    });
                }}
            />
        );
    }

    //----------------------------------------------------------------//
    // renderTransactions () {

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
}

export default withAppStateAndUser ( AccountScreen );
