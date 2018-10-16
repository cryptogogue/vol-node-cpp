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
    // Fetch account balance from the blockchain
    getAccountBalance () {

        let miners = {};
        let minerQueue = [];

        let checkIsMiner = ( url ) => {

            return new Promise (( resolve ) => {

                if ( url in miners ) resolve ( miners [ url ]);

                let isMiner = false;

                this.revocablePromise ( fetch ( url ))
                .then (( response ) => { return response.json (); })
                .then (( data ) => { isMiner = ( data.type === 'VOL_MINING_NODE' ); })
                .catch (( error ) => { console.log ( error )})
                .finally (() => {
                    miners [ url ] = isMiner;
                    resolve ( isMiner );
                });
            });
        }

        let updateBalance = ( url ) => {

            return new Promise (( resolve ) => {

                let balance = false;
                let nonce = false;

                this.revocablePromise ( fetch ( url + '/accounts/' + this.state.accountId ))
                .then (( response ) => { return response.json (); })
                .then (( data ) => {
                    if ( data.account && ( data.account.accountName === this.state.accountId )) {
                        balance = data.account.balance;
                        nonce = data.account.nonce;
                        this.setState ({ balance: balance });
                        this.setState ({ nonce: nonce });
                    }
                })
                .catch (( error ) => { console.log ( error )})
                .finally (() => {
                    resolve ({ balance: balance, nonce: nonce });
                });
            });
        }

        let fetchBalance = async () => {

            if ( minerQueue.length === 0 ) {
                const { nodes } = this.props.appState.state;
                for ( let i in nodes ) {

                    const url = nodes [ i ];
                    if ( await checkIsMiner ( url )) {
                        minerQueue.push ( url );
                    }
                }
            }

            if ( !this.state.accountId ) {
                minerQueue = [];
            }

            if ( minerQueue.length > 0 ) {
                const url = minerQueue.shift ();
                await updateBalance ( url );
            }

            // it's possible to be a bit smarter about when this kicks off. but...
            // not sure if it's worth the effort.
            this.revocableTimeout (() => { fetchBalance (); }, 1000 );
        }

        this.revocableTimeout (() => { fetchBalance (); }, 0 );
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

        return (
            <div>
                <Grid textAlign = "center" style = {{ height: '100%' }} verticalAlign = "middle">
                    <Grid.Column style = {{ maxWidth: 450 }}>

                        <NavigationBar navTitle = "Accounts"/>

                        <div>
                            { this.renderAccountSelector ()}
                            { this.renderAccountDetails ()}
                        </div>

                        <Segment>
                            <TransactionFormSelector
                                accountId = { targetAccountId }
                                nonce = { this.state.nonce }
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

        return (
            <Segment>
                { transactions.map (( transaction ) => {
                    return (<p key = { count++ }>{ transaction.friendlyName }</p>);
                })}
            </Segment>
        );
    }
}

export default withAppState ( AccountScreen );
