/* eslint-disable no-whitespace-before-property */

import { withAppState }         from './AppStateProvider';
import BaseComponent            from './BaseComponent';
import NavigationBar            from './NavigationBar';
import NodeListView             from './NodeListView';
import TransactionFormSelector  from './TransactionFormSelector';
import React                    from 'react';
import { Redirect }             from 'react-router-dom';
import { Dropdown, Segment, Header, Icon, Divider, Modal, Grid } from 'semantic-ui-react';

//================================================================//
// AccountSelection
//================================================================//
class AccountScreen extends BaseComponent {
    
    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        this.state = {
            accountId : this.props.match.params && this.props.match.params.accountId,
            balance : -1,
            nonce: -1,
        };

        this.getAccountBalance ();
    }

    //----------------------------------------------------------------//
    async getAccountBalance () {

        let balanceHistogram = new Map ();
        let bestNonce = 0;

        let updateBalance = async ( url ) => {

            try {
                const data = await ( await this.revocableFetch ( url + '/accounts/' + this.state.accountId )).json ();

                if ( data.account && ( data.account.accountName === this.state.accountId )) {

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

        if ( this.state.accountId ) {

            const { minerURLs } = this.props.appState;

            let promises = [];
            minerURLs.forEach (( url ) => {
                promises.push ( updateBalance ( url ));
            });
            await Promise.all ( promises );
        
            let bestBalanceCount = 0;
            let bestBalance = -1;

            balanceHistogram.forEach (( balanceCount, balance ) => {

                if ( bestBalanceCount < balanceCount ) {
                    bestBalance = balance;
                }
            });

            this.setState ({ balance: bestBalance });
            this.setState ({ nonce: bestNonce });
        }
        this.revocableTimeout (() => { this.getAccountBalance ()}, 1000 );
    }

    //----------------------------------------------------------------//
    render () {

        const { appState } = this.props;

        // TODO: move redirects to a HOC
        if ( !appState.hasUser ()) return ( <Redirect to = "/" />);
        if ( !appState.isLoggedIn ()) return ( <Redirect to = "/login" />);

        let targetAccountId = this.state.accountId;
        const urlAccountId = this.props.match.params && this.props.match.params.accountId;

        if ( !targetAccountId ) {
            const accountNames = Object.keys ( appState.state.accounts );
            targetAccountId = accountNames && accountNames.length && accountNames [ 0 ];
        }

        if ( targetAccountId && ( targetAccountId !== urlAccountId )) {
            return ( <Redirect to = { '/accounts/' + targetAccountId }/>);
        }

        // TODO: nonce should come from account or from last known transaction
        const nonce = this.state.nonce < 0 ? 0 : this.state.nonce;

        return (
            <div>
                <Grid textAlign = "center" style = {{ height: '100%' }} verticalAlign = "middle">
                    <Grid.Column style = {{ maxWidth: 450 }}>

                        <NavigationBar navTitle = "Accounts"/>

                        <div>
                            <p>ACTIVE MINERS: { appState.state.activeMinerCount }</p>
                            <p>ACTIVE MARKETS: { appState.state.activeMarketCount }</p>
                        </div>

                        <div>
                            { this.renderAccountSelector ()}
                            { this.renderAccountDetails ()}
                        </div>

                        <Segment>
                            <TransactionFormSelector
                                accountId = { targetAccountId }
                                nonce = { nonce }
                            />
                        </Segment>

                        { this.renderTransactions ()}

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

        const accountId = this.props.match.params.accountId;
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
                onChange = {( event, data ) => { this.setState ({ accountId : data.value, balance : -1 }); }}
            />
        );
    }

    //----------------------------------------------------------------//
    renderTransactions () {

        const { transactions } = this.props.appState.state;
        if ( transactions.length === 0 ) return;

        let count = 0;

        // transaction type     nonce       pending     rejected        confirmed

        return (
            <Segment>
                { transactions.map (( entry ) => {
                    return (<p key = { count++ }>{ entry.transaction.friendlyName }</p>);
                })}
            </Segment>
        );
    }
}

export default withAppState ( AccountScreen );
