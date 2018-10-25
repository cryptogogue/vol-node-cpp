/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import BaseComponent        from './BaseComponent';
import * as storage         from './utils/storage';
import React                from 'react';

const STORE_ACCOUNTS        = 'vol_accounts';
const STORE_NODES           = 'vol_nodes';
const STORE_PASSWORD_HASH   = 'vol_password_hash';
const STORE_SESSION         = 'vol_session';
const STORE_TRANSACTIONS    = 'vol_transactions';

const TRANSACTION_STATUS_PENDING    = 'pending';
// const TRANSACTION_STATUS_SUBMITTED  = 'submitted';
// const TRANSACTION_STATUS_CONFIRMED  = 'confirmed';

const TheContext = React.createContext ();
const AppStateConsumer = TheContext.Consumer;

//----------------------------------------------------------------//
function withAppState ( WrappedComponent ) {

    return class extends React.Component {

        render () {
            return (
                <AppStateConsumer>
                    {({ appState }) => (
                        <WrappedComponent appState = { appState }{ ...this.props }/>
                    )}
                </AppStateConsumer>
            );
        }
    };
}

//================================================================//
// AppStateProvider
//================================================================//
class AppStateProvider extends BaseComponent {

    //----------------------------------------------------------------//
    affirmNodeURL ( nodeURL ) {

        const idx = this.state.nodes.indexOf ( nodeURL );
        if ( idx < 0 ) {
            let nodes = this.state.nodes.slice ( 0 );
            nodes.push ( nodeURL );
            storage.setItem ( STORE_NODES, nodes );
            this.setState ({ nodes : nodes });
        }
    }

    //----------------------------------------------------------------//
    clearNodeURLs () {
        storage.setItem ( STORE_NODES, []);
        this.setState ({ nodes : []});
    }

    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        this.nodes = {};
        this.miners = [];
        this.markets = [];

        this.state = this.loadState ();

        this.discoverNetwork ();
    }

    //----------------------------------------------------------------//
    deleteStorage () {
        storage.clear ();
        this.setState ( this.makeClearState ());
    }

    //----------------------------------------------------------------//
    async discoverNetwork () {

        let discoverNode = async ( url ) => {

            try {
                const data = await ( await this.revocableFetch ( url )).json ();

                let node = {
                    url: url,
                    type: data.type,
                };
                this.nodes [ url ] = node;

                if ( data.type === 'VOL_MINING_NODE' ) {
                    this.miners.push ( node );
                }
                if ( data.type === 'VOL_PROVIDER' ) {
                    this.markets.push ( node );
                }
            }
            catch ( error ) {
                console.log ( error );
            }
        }

        const { nodes } = this.state;

        let promises = [];
        for ( let i in nodes ) {

            let url = nodes [ i ];

            if ( !( url in this.nodes )) {
                promises.push ( discoverNode ( url ));
            }
        }
        await Promise.all ( promises );

        if ( promises.length ) {
            this.setState ({
                activeMinerCount: this.miners.length,
                activeMarketCount: this.markets.length
            });
        }

        this.revocableTimeout (() => { this.discoverNetwork ()}, 1000 );
    }

    //----------------------------------------------------------------//
    getAccount ( accountId ) {
        const { accounts } = this.state;
        return ( accountId in accounts ) && accounts [ accountId ];
    }

    //----------------------------------------------------------------//
    getAccountKeyNames ( accountId ) {
        const account = this.getAccount ( accountId );
        return ( account && Object.keys ( account.keys )) || [];
    }

    //----------------------------------------------------------------//
    getDefaultAccountKeyName ( accountId, defaultKeyName ) {
        const accountKeyNames = this.getAccountKeyNames ( accountId );
        if ( defaultKeyName && accountKeyNames.includes [ defaultKeyName ]) return defaultKeyName;
        return (( accountKeyNames.length > 0 ) && accountKeyNames [ 0 ]) || '';
    }

    //----------------------------------------------------------------//
    hasUser () {
        return ( this.state.user.passwordHash.length > 0 );
    }

    //----------------------------------------------------------------//
    isLoggedIn () {
        return ( this.state.session.isLoggedIn === true );
    }

    //----------------------------------------------------------------//
    loadState () {

        let state = this.makeClearState ();

        state.user.passwordHash     = storage.getItem ( STORE_PASSWORD_HASH ) || state.user.passwordHash;
        state.accounts              = storage.getItem ( STORE_ACCOUNTS ) || state.accounts;
        state.nodes                 = storage.getItem ( STORE_NODES ) || state.nodes;
        state.session               = storage.getItem ( STORE_SESSION ) || state.session;
        state.transactions          = storage.getItem ( STORE_TRANSACTIONS ) || state.transactions;

        return state;
    }

    //----------------------------------------------------------------//
    // LOGIN to the app
    // TODO: Pass clear errors/error messages
    login ( status ) {

        const session = {
            isLoggedIn : status,
        }

        storage.setItem ( STORE_SESSION, session );
        this.setState ({ session : session });
    }

    //----------------------------------------------------------------//
    // CLEAR App State (reset to initial state)
    makeClearState () {

        let state = {
            accounts: {},
            activeMarketCount: 0,
            activeMinerCount: 0,
            session: {
                isLoggedIn: false,
            },
            nodes: [],
            transactions: [],
            user: {
                passwordHash: '',
                confirmPassword: '',
                password: ''
            }
        };

        return state;
    }

    //----------------------------------------------------------------//
    pushTransaction ( transaction ) {

        let entry = {
            transaction:    transaction,
            status:         TRANSACTION_STATUS_PENDING,
        };

        let transactions = this.state.transactions.slice ( 0 );
        transactions.push ( entry );
        storage.setItem ( STORE_TRANSACTIONS, transactions );
        this.setState ({ transactions : transactions });
    }

    //----------------------------------------------------------------//
    register ( passwordHash ) {

        const session = {
            isLoggedIn : true,
        }

        storage.setItem ( STORE_PASSWORD_HASH, passwordHash );
        storage.setItem ( STORE_SESSION, session );

        this.setState ({
            session : session,
            user : {
                passwordHash : passwordHash,
                password : '',
                confirmPassword : '',
            }
        });
    }
    
    //----------------------------------------------------------------//
    render () {
        return (
            <TheContext.Provider
                value = {{ appState: this }}>
                { this.props.children }
            </TheContext.Provider>
        )
    }

    //----------------------------------------------------------------//
    saveAccount ( accountId, privateKey, publicKey ) {

        let account = {
            keys : {
                master : {
                    privateKey : privateKey, // TODO: encrypt this with password
                    publicKey : publicKey
                }
            },
            isNew : true
        };

        // Add new account to StateManager state
        let accounts = this.state.accounts;
        accounts = Object.assign ({[ accountId ] : account }, accounts );

        // Save new account to local storage
        storage.setItem ( STORE_ACCOUNTS, accounts );

        this.setState ({
            accounts : accounts
        });
    }
}

export { AppStateProvider, AppStateConsumer, withAppState };