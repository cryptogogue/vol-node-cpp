/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import BaseComponent        from './BaseComponent';
import * as storage         from './utils/storage';
import React                from 'react';

const STORE_ACCOUNTS        = '.vol_accounts';
const STORE_NODES           = '.vol_nodes';
const STORE_PASSWORD_HASH   = '.vol_password_hash';
const STORE_SESSION         = '.vol_session';
const STORE_TRANSACTIONS    = '.vol_transactions';

const TRANSACTION_STATUS_PENDING    = 'pending';
const TRANSACTION_STATUS_DONE       = 'done';

const TheContext = React.createContext ();
const AppStateConsumer = TheContext.Consumer;

//----------------------------------------------------------------//
function withAppState ( WrappedComponent ) {

    return class extends React.Component {

        //----------------------------------------------------------------//
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

//----------------------------------------------------------------//
function withAppStateAndUser ( WrappedComponent ) {

    let DerivedComponent = class extends WrappedComponent {

        //----------------------------------------------------------------//
        constructor ( props ) {
            super ( props );

            let { appState } = this.props;
            appState.setUser ( this.props.match.params && this.props.match.params.userId );
        }
    }

    return class extends React.Component {

        //----------------------------------------------------------------//
        render () {
            return (
                <AppStateConsumer>
                    {({ appState }) => (
                        <DerivedComponent appState = { appState }{ ...this.props }/>
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
    affirmAccountAndKey ( accountId, keyName, privateKey, publicKey ) {

        let accounts = this.state.accounts;

        let account = accounts [ accountId ] || { keys: {}};

        let key = account.keys [ keyName ] || {};

        key.privateKey = privateKey;
        key.publicKey = publicKey;

        account.keys [ keyName ] = key;

        // Add new account to StateManager state
        accounts = Object.assign ({[ accountId ] : account }, accounts );

        // Save new account to local storage
        this.setItem ( STORE_ACCOUNTS, accounts );

        this.setState ({
            accounts : accounts
        });
    }

    //----------------------------------------------------------------//
    affirmNodeURL ( nodeURL ) {

        const idx = this.state.nodes.indexOf ( nodeURL );
        if ( idx < 0 ) {
            let nodes = this.state.nodes.slice ( 0 );
            nodes.push ( nodeURL );
            this.setItem ( STORE_NODES, nodes );
            this.setState ({ nodes : nodes });
        }
    }

    //----------------------------------------------------------------//
    clearNodeURLs () {
        this.setItem ( STORE_NODES, []);
        this.setState ({ nodes : []});
    }

    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        this.minerURLs = new Set ();
        this.marketURLs = new Set ();
        this.urlBackoff = {};

        this.state = this.makeClearState ();

        this.discoverNetwork ();
        this.processTransactions ();
    }

    //----------------------------------------------------------------//
    deleteStorage () {

        const userId = this.state.userId;

        storage.clear ();
        this.setState ( this.makeClearState ( userId ));
    }

    //----------------------------------------------------------------//
    deleteUserStorage () {

        const userId = this.state.userId;

        storage.removeItem ( this.prefixStoreKey ( STORE_ACCOUNTS ));
        storage.removeItem ( this.prefixStoreKey ( STORE_NODES ));
        storage.removeItem ( this.prefixStoreKey ( STORE_PASSWORD_HASH ));
        storage.removeItem ( this.prefixStoreKey ( STORE_SESSION ));
        storage.removeItem ( this.prefixStoreKey ( STORE_TRANSACTIONS ));

        this.setState ( this.makeClearState ( userId ));
    }

    //----------------------------------------------------------------//
    async discoverNetwork ( count ) {

        count = count || 1;

        const { nodes } = this.state;

        if ( nodes.length === 0 ) {
            this.minerURLs.clear ();
            this.marketURLs.clear ();
        }

        let removeNode = ( url ) => {
            this.minerURLs.delete ( url );
            this.marketURLs.delete ( url );
        }

        let discoverNode = async ( url ) => {

            let backoff = this.urlBackoff [ url ];
            if ( backoff ) {
                backoff.counter = backoff.counter + 1;
                if ( backoff.counter < backoff.wait ) return;
            }

            try {
                const data = await this.revocableFetchJSON ( url );
                removeNode ( url );

                if ( data.type === 'VOL_MINING_NODE' ) {
                    this.minerURLs.add ( url );
                }
                if ( data.type === 'VOL_PROVIDER' ) {
                    this.marketURLs.add ( url );
                }

                delete this.urlBackoff [ url ];
            }
            catch ( error ) {

                this.urlBackoff [ url ] = {
                    counter: 0,
                    wait : backoff ? backoff.wait * 2 : 1,
                }
                removeNode ( url );
                console.log ( error );
            }
        }

        let promises = [];
        for ( let i in nodes ) {
            let url = nodes [ i ];
            promises.push ( discoverNode ( url ));
        }
        await this.revocableAll ( promises );

        this.revocableSetState ({
            activeMinerCount: this.minerURLs.size,
            activeMarketCount: this.marketURLs.size,
        });

        this.revocableTimeout (() => { this.discoverNetwork ( count + 1 )}, 1000 );
    }

    //----------------------------------------------------------------//
    findAccountIdByPublicKey ( publicKey ) {

        const accounts = this.state.accounts;
        for ( let accountId in accounts ) {
            const account = accounts [ accountId ];
            for ( let keyName in account.keys ) {
                const key = account.keys [ keyName ];
                if ( key.publicKey === publicKey ) return accountId;
            }
        }
        return false;
    }

    //----------------------------------------------------------------//
    finishTransaction ( accountId, nonce ) {

        let { transactions } = this.state;

        if ( !( accountId in transactions )) return;

        let transaction = this.state.transactions [ accountId ];
        if ( transaction.status !== TRANSACTION_STATUS_PENDING ) return;
        if ( nonce <= transaction.fieldValues.makerNonce ) return;

        transaction.status = Object.assign ({},
            transaction,
            { status: TRANSACTION_STATUS_DONE }
        );

        transactions = Object.assign ({},
            this.state.transactions,
            {[ accountId ] : transaction }
        );

        this.setItem ( STORE_TRANSACTIONS, transactions );
        this.setState ({ transactions : transactions });
    }

    //----------------------------------------------------------------//
    formatTransaction ( format, fieldValues ) {

        let result = {};
        Object.keys ( format ).forEach (( fieldName ) => {

            const fieldSource = format [ fieldName ];
            let fieldValue;

            if (( typeof fieldSource ) === 'object' ) {
                fieldValue = this.formatTransaction ( fieldSource, fieldValues );
            }
            else {
                fieldValue = fieldValues [ fieldSource ];
            }
            result [ fieldName ] = fieldValue;
        });

        return result;
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
    getPendingTransaction ( accountId ) {

        if ( !( accountId in this.state.transactions )) return;
        const transaction = this.state.transactions [ accountId ];
        return ( transaction.status === TRANSACTION_STATUS_PENDING ) && transaction;
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
    loadState ( userId ) {

        userId = userId || '';

        let state = this.makeClearState ();

        state.user.passwordHash     = storage.getItem ( userId + STORE_PASSWORD_HASH ) || state.user.passwordHash;
        state.accounts              = storage.getItem ( userId + STORE_ACCOUNTS ) || state.accounts;
        state.nodes                 = storage.getItem ( userId + STORE_NODES ) || state.nodes;
        state.session               = storage.getItem ( userId + STORE_SESSION ) || state.session;
        state.transactions          = storage.getItem ( userId + STORE_TRANSACTIONS ) || state.transactions;
        state.userId                = userId;

        console.log ( 'LOADED STATE:', state );

        return state;
    }

    //----------------------------------------------------------------//
    // LOGIN to the app
    // TODO: Pass clear errors/error messages
    login ( status ) {

        const session = this.makeSession ( status );

        this.setItem ( STORE_SESSION, session );
        this.setState ({ session : session });
    }

    //----------------------------------------------------------------//
    // CLEAR App State (reset to initial state)
    makeClearState ( userId ) {

        let state = {
            accounts: {},
            activeMarketCount: 0,
            activeMinerCount: 0,
            session: this.makeSession ( false ),
            nodes: [],
            transactions: [],
            user: {
                passwordHash: '',
                confirmPassword: '',
                password: ''
            },
            userId: userId || false,
        };

        return state;
    }

    //----------------------------------------------------------------//
    makeSession ( isLoggedIn ) {
        return { isLoggedIn: isLoggedIn };
    }

    //----------------------------------------------------------------//
    prefixStoreKey ( key ) {
        return this.state.userId + key;
    }

    //----------------------------------------------------------------//
    async processTransactions () {

        const { transactions } = this.state;

        let submitTransaction = async ( transaction, url ) => {

            try {

                let body = this.formatTransaction ( transaction.schema.format, transaction.fieldValues );
                body.type = transaction.schema.transactionType;

                await this.revocableFetch ( url + '/transactions', {
                    method : 'POST',
                    headers : { 'content-type': 'application/json' },
                    body : JSON.stringify ( body )
                });
            }
            catch ( error ) {
                console.log ( error );
            }
        }

        let promises = [];
        for ( let accountName in transactions ) {

            const transaction = transactions [ accountName ];

            if ( transaction.status === TRANSACTION_STATUS_PENDING ) {
                
                this.minerURLs.forEach (( url ) => {
                    promises.push ( submitTransaction ( transaction, url ));
                });
            }
        }
        await this.revocableAll ( promises );

        this.revocableTimeout (() => { this.processTransactions ()}, 1337 );
    }

    //----------------------------------------------------------------//
    register ( passwordHash ) {

        const session = this.makeSession ( true );

        this.setItem ( STORE_PASSWORD_HASH, passwordHash );
        this.setItem ( STORE_SESSION, session );

        // defaults
        // TODO: remove
        const nodes = [
            'http://localhost:9090',
            'http://localhost:7777',
        ];

        this.setItem ( STORE_NODES, nodes );

        this.setState ({
            nodes: nodes,
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

        this.affirmAccountAndKey ( accountId, 'master', privateKey, publicKey );
    }

    //----------------------------------------------------------------//
    setItem ( key, value ) {
        return storage.setItem ( this.prefixStoreKey ( key ), value );
    }

    //----------------------------------------------------------------//
    setUser ( userId ) {

        userId = userId || '';

        if ( this.state.userId !== userId ) {

            console.log ( 'CHANGED USER:', userId );

            // let session = this.makeSession ( false );

            // storage.setItem ( this.state.userId + STORE_SESSION, session );
            // storage.setItem ( userId + STORE_SESSION, session );

            this.setState ( this.loadState ( userId ));
        }
    }

    //----------------------------------------------------------------//
    startTransaction ( schema, fieldValues ) {

        let transaction = {
            status:             TRANSACTION_STATUS_PENDING,
            submitted:          0,
            schema:             schema,
            fieldValues:        fieldValues,
        }

        let transactions = Object.assign ({},
            this.state.transactions,
            {[ fieldValues.makerAccountName ] : transaction }
        );

        console.log ( 'START TRANSACTION', transactions );

        this.setItem ( STORE_TRANSACTIONS, transactions );
        this.setState ({ transactions : transactions });
    }
}

export { AppStateProvider, AppStateConsumer, withAppState, withAppStateAndUser };