/* eslint-disable no-whitespace-before-property */

import { withAppState }         from './AppStateProvider';
import NavigationBar            from './NavigationBar';
//import Transactions             from './Transactions';
import { request }              from './utils/api.js';
import React, { Component }     from 'react';
import { Redirect }             from 'react-router-dom';
import { Dropdown, Segment, Header, Icon, Button, Divider, Modal, Grid } from 'semantic-ui-react';

//================================================================//
// AccountSelection
//================================================================//
class AccountScreen extends Component {
    
    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        console.log ( 'CONSTRUCTOR' );

        this.state = {
            accountId : this.props.match.params && this.props.match.params.accountId
        };

        this.getAccountBalance  = this.getAccountBalance.bind ( this );
        this.openAccount        = this.openAccount.bind ( this );
    }

    //----------------------------------------------------------------//
    // Fetch account balance from the blockchain
    getAccountBalance () {

        const URI = 'http://localhost:9090/accounts/' + this.state.accountName;
        
        request ( URI )
        .then ( res => { return res.json (); })
        .then ( data => { this.setState ({ balance : data.account.balance }) })
        .catch ( error => this.setState ({ error : error.message, balance : "--"}));
    }

    //----------------------------------------------------------------//
    // Perform OPEN_ACCOUNT transaction on the blockchain
    openAccount () {
        
        const accountName = this.state.accountName;
        const publicKey = this.state.publicKey;

        let accountObject = {
            "type" : "OPEN_ACCOUNT",
            "maker" : {
                "accountName" : "9090",
                "keyName" : "master",
                "gratuity" : 0,
                "nonce" : 0
            },
            "accountName" : accountName,
            "key" : {
                "type" : "EC_HEX",
                "groupName" : "secp256k1",
                "publicKey" : publicKey
            },
            "keyName" : "master",
            "amount" : 5
        };

        request ( `http://localhost:9090/transactions`, 'post', accountObject )
        .catch ( error => console.log ( error ));
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
                            { this.renderDropdown ()}
                            { this.renderAccountDetails ()}
                        </div>

                    </Grid.Column>
                </Grid>
            </div>
        );
    }

    //----------------------------------------------------------------//
    renderAccountDetails = () => {

        const accountId = this.props.match.params.accountId;
        const { accounts } = this.props.appState.state;
        const account = ( accountId in accounts ) && accounts [ accountId ];

        if ( !account ) return;

        const publicKey = account.keys.master.publicKey;

        return (
            <Segment>
                <Header as = "h2" icon>

                    <Icon name = "key" circular />
                    
                    { accountId }

                    <Modal size = "small" trigger = { <Header.Subheader>{ publicKey && publicKey.substr ( 0,30 ) + "..." }</Header.Subheader> }>
                        <Modal.Content>
                            <center>
                                <h3>Public Key</h3>
                                <Divider />
                                <p>{ publicKey }</p>
                            </center>
                        </Modal.Content>
                    </Modal>

                    <Button primary onClick = { this.getAccountBalance }>
                        Check balance
                    </Button>

                    <p>Balance: { this.state.balance }</p>

                </Header>
                <Divider hidden />
                <Button primary icon labelPosition = "right" onClick = { this.openAccount }>
                    Push account to chain
                    <Icon name = "right arrow" />
                </Button>
            </Segment>
        );
    }

    //----------------------------------------------------------------//
    renderDropdown () {

        const { accounts } = this.props.appState.state;
        let options = [];

        Object.keys ( accounts ).forEach ( function ( accountId ) {
            options.push ({ key:accountId, value:accountId, text:accountId })
        });

        return (
            <Dropdown 
                placeholder = "Select Account"
                fluid
                search
                selection
                options = { options }
                onChange = {( event, data ) => { this.setState ({ accountId : data.value }); }}
            />
        );
    }
}

export default withAppState ( AccountScreen );
