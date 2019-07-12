/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import * as storage         from '../utils/storage';
import { Store }            from './Store';
import { action, computed, extendObservable, observe, observable } from 'mobx';
import React                from 'react';
import { Redirect }         from 'react-router';

const STORE_ACCOUNTS        = '.vol_accounts';
const STORE_NODE            = '.vol_node';
const STORE_NODES           = '.vol_nodes';
const STORE_PASSWORD_HASH   = '.vol_password_hash';
const STORE_SESSION         = '.vol_session';
const STORE_TRANSACTIONS    = '.vol_transactions';

const TRANSACTION_STATUS_PENDING    = 'pending';
const TRANSACTION_STATUS_DONE       = 'done';

export const NODE_TYPE = {
    UNKNOWN:    'UNKNOWN',
    MINING:     'MINING',
    PROVIDER:   'PROVIDER',
};

export const NODE_STATUS = {
    UNKNOWN:    'UNKNOWN',
    ONLINE:     'ONLINE',
    OFFLINE:    'OFFLINE',
};

//================================================================//
// AppStateStore
//================================================================//
export class AppStateStore extends Store {

    @observable userId;
    @observable accountId;
    @observable accountInfo;

    // persisted
    @observable accounts;
    @observable node;
    @observable nodes;
    @observable passwordHash;
    @observable session;
    @observable transactions;

    //----------------------------------------------------------------//
    @computed get
    account () {
        return this.getAccount ();
    }

    //----------------------------------------------------------------//
    @computed get
    accountKeyNames () {
        const account = this.account
        return ( account && Object.keys ( account.keys )) || [];
    }

    //----------------------------------------------------------------//
    @computed get
    activeMarketCount () {
        
        let count = 0;
        for ( let url in this.nodes ) {
            const info = this.nodes [ url ];
            if (( info.type === NODE_TYPE.PROVIDER ) && ( info.status === NODE_STATUS.ONLINE )) {
                count++;
            }
        }
        return count;
    }

    //----------------------------------------------------------------//
    @computed get
    activeMinerCount () {
        
        let count = 0;
        for ( let url in this.nodes ) {
            const info = this.nodes [ url ];
            if (( info.type === NODE_TYPE.MINING ) && ( info.status === NODE_STATUS.ONLINE )) {
                count++;
            }
        }
        return count;
    }

    //----------------------------------------------------------------//
    @action
    affirmAccountAndKey ( accountId, keyName, privateKey, publicKey ) {

        let accounts = this.accounts;

        let account = accounts [ accountId ] || { keys: {}};

        let key = account.keys [ keyName ] || {};

        key.privateKey = privateKey;
        key.publicKey = publicKey;

        account.keys [ keyName ] = key;

        // Add new account to StateManager state
        this.accounts [ accountId ] = account;
    }

    //----------------------------------------------------------------//
    @action
    affirmLocalhostNodes () {

        console.log ( 'AFFIRM LOCALHOST NODE LIST' );

        this.affirmNodeURL ( 'http://localhost:9090' );
        this.affirmNodeURL ( 'http://localhost:7777' );
    }

    //----------------------------------------------------------------//
    @action
    affirmNodeURL ( nodeURL ) {

        if ( !( nodeURL in this.nodes )) {
            this.nodes [ nodeURL ] = this.makeNodeInfo ();

            if ( Object.keys ( this.nodes ).length === 1 ) {
                this.node = nodeURL;
            }
        }
    }

    //----------------------------------------------------------------//
    affirmObservers () {

        this.observeMember ( 'accounts',        () => { this.persistValue ( STORE_ACCOUNTS, this.accounts )});
        this.observeMember ( 'node',            () => { this.persistValue ( STORE_NODE, this.node )});
        this.observeMember ( 'nodes',           () => { this.persistValue ( STORE_NODES, this.nodes )});
        this.observeMember ( 'passwordHash',    () => { this.persistValue ( STORE_PASSWORD_HASH, this.passwordHash )});
        this.observeMember ( 'session',         () => { this.persistValue ( STORE_SESSION, this.session )});
        this.observeMember ( 'transactions',    () => { this.persistValue ( STORE_TRANSACTIONS, this.transactions )});
    }

    //----------------------------------------------------------------//
    @computed get
    balance () {
        return this.accountInfo.balance;
    }

    //----------------------------------------------------------------//
    // CLEAR App State (reset to initial state)
    @action
    clearState () {

        this.accounts           = {};
        this.session            = this.makeSession ( false );
        this.node               = '';
        this.nodes              = {};
        this.passwordHash       = '';
        this.transactions       = [];

        this.setAccountInfo ();
    }

    //----------------------------------------------------------------//
    constructor ( userId, accountId ) {
        super ();

        this.minerURLs = new Set ();
        this.marketURLs = new Set ();
        this.urlBackoff = {};

        this.loadState ( userId, accountId );

        // this.discoverNetwork ();
        // this.processTransactions ();
    }

    //----------------------------------------------------------------//
    @action
    deleteAccount () {

        if ( this.accountId in this.accounts ) {
            delete this.accounts [ this.accountId ];
        }
        this.accountId = '';
        this.setAccountInfo ();
    }

    //----------------------------------------------------------------//
    @action
    deleteNodeList () {

        console.log ( 'DELETE NODE LIST' );

        this.node = '';
        this.nodes = {};
    }

    //----------------------------------------------------------------//
    deleteStorage () {

        storage.clear ();

        this.disposeObservers ();
        this.clearState ();
        this.affirmObservers ();
    }

    //----------------------------------------------------------------//
    deleteUserStorage () {

        storage.removeItem ( this.prefixStoreKey ( STORE_ACCOUNTS ));
        storage.removeItem ( this.prefixStoreKey ( STORE_NODE ));
        storage.removeItem ( this.prefixStoreKey ( STORE_NODES ));
        storage.removeItem ( this.prefixStoreKey ( STORE_PASSWORD_HASH ));
        storage.removeItem ( this.prefixStoreKey ( STORE_SESSION ));
        storage.removeItem ( this.prefixStoreKey ( STORE_TRANSACTIONS ));

        this.disposeObservers ();
        this.clearState ();
        this.affirmObservers ();
    }

    //----------------------------------------------------------------//
    findAccountIdByPublicKey ( publicKey ) {

        const accounts = this.accounts;
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

        let transactions = this.transactions;

        if ( !( accountId in transactions )) return;

        let transaction = this.state.transactions [ accountId ];
        if ( transaction.status !== TRANSACTION_STATUS_PENDING ) return;
        if ( nonce <= transaction.fieldValues.makerNonce ) return;

        transaction.status = Object.assign ({},
            transaction,
            { status: TRANSACTION_STATUS_DONE }
        );

        transactions = Object.assign ({},
            this.transactions,
            {[ accountId ] : transaction }
        );

        this.persistValue ( STORE_TRANSACTIONS, transactions );
        this.transactions = transactions;
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
        accountId = accountId || this.accountId;
        const accounts = this.accounts;
        return ( accountId in accounts ) && accounts [ accountId ] || null;
    }

    //----------------------------------------------------------------//
    getAccountIdOrDefault () {

        if ( !accountId ) {
            const accountNames = Object.keys ( appState.accounts );
            accountId = accountNames && accountNames.length && accountNames [ 0 ];
        }
    }

    //----------------------------------------------------------------//
    getDefaultAccountKeyName ( defaultKeyName ) {
        const accountKeyNames = this.accountKeyNames ;
        if ( defaultKeyName && accountKeyNames.includes [ defaultKeyName ]) return defaultKeyName;
        return (( accountKeyNames.length > 0 ) && accountKeyNames [ 0 ]) || '';
    }

    //----------------------------------------------------------------//
    getMinerURL () {

        return this.nodes [ 0 ]; // this is a hack for now (until we redo the config)
    }

    //----------------------------------------------------------------//
    getNodeInfo ( url ) {

        return this.nodes [ url ] || this.makeNodeInfo ();
    }

    //----------------------------------------------------------------//
    getPendingTransaction ( accountId ) {

        if ( !( accountId in this.transactions )) return;
        const transaction = this.transactions [ accountId ];
        return ( transaction.status === TRANSACTION_STATUS_PENDING ) && transaction;
    }

    //----------------------------------------------------------------//
    hasUser () {
        return ( this.passwordHash.length > 0 );
    }

    //----------------------------------------------------------------//
    isLoggedIn () {
        return ( this.session.isLoggedIn === true );
    }

    //----------------------------------------------------------------//
    @action
    loadState ( userId, accountId ) {

        this.disposeObservers ();
        this.clearState ();

        userId = userId || '';
        this.userId = userId;

        this.accounts               = storage.getItem ( userId + STORE_ACCOUNTS ) || this.accounts;
        this.node                   = storage.getItem ( userId + STORE_NODE ) || this.node;
        this.nodes                  = storage.getItem ( userId + STORE_NODES ) || this.nodes;
        this.passwordHash           = storage.getItem ( userId + STORE_PASSWORD_HASH ) || this.passwordHash;
        this.session                = storage.getItem ( userId + STORE_SESSION ) || this.session;
        this.transactions           = storage.getItem ( userId + STORE_TRANSACTIONS ) || this.transactions;

        for ( let url in this.nodes ) {
            this.nodes [ url ].status = NODE_STATUS.UNKNOWN;
        }

        const accountNames = Object.keys ( this.accounts );
        this.accountId = (( accountId in this.accounts ) && accountId ) || ( accountNames.length && accountNames [ 0 ]) || '';

        this.affirmObservers ();
    }

    //----------------------------------------------------------------//
    @action
    login ( status ) {

        this.session = this.makeSession ( status );
    }

    //----------------------------------------------------------------//
    makeNodeInfo ( type, status ) {

        return {
            type:       type || NODE_TYPE.UNKNOWN,
            status:     status || NODE_STATUS.UNKNOWN,
        };
    }

    //----------------------------------------------------------------//
    makeSession ( isLoggedIn ) {
        return { isLoggedIn: isLoggedIn };
    }

    //----------------------------------------------------------------//
    @computed get
    nonce () {
        return this.accountInfo.nonce;
    }

    //----------------------------------------------------------------//
    persistValue ( key, value ) {
        return storage.setItem ( this.prefixStoreKey ( key ), value );
    }

    //----------------------------------------------------------------//
    prefixStoreKey ( key ) {
        return this.userId + key;
    }

    //----------------------------------------------------------------//
    prefixURL ( url ) {

        const userId = this.userId;
        if ( userId && userId.length ) {
            return '/' + userId + url;
        }
        return url;
    }

    //----------------------------------------------------------------//
    async processTransactions () {

        // const { transactions } = this.state;

        // let submitTransaction = async ( transaction, url ) => {

        //     try {

        //         let body = this.formatTransaction ( transaction.schema.format, transaction.fieldValues );
        //         body.type = transaction.schema.transactionType;

        //         await this.revocableFetch ( url + '/transactions', {
        //             method : 'POST',
        //             headers : { 'content-type': 'application/json' },
        //             body : JSON.stringify ( body )
        //         });
        //     }
        //     catch ( error ) {
        //         console.log ( error );
        //     }
        // }

        // let promises = [];
        // for ( let accountName in transactions ) {

        //     const transaction = transactions [ accountName ];

        //     if ( transaction.status === TRANSACTION_STATUS_PENDING ) {
                
        //         this.minerURLs.forEach (( url ) => {
        //             promises.push ( submitTransaction ( transaction, url ));
        //         });
        //     }
        // }
        // await this.revocableAll ( promises );

        // this.revocableTimeout (() => { this.processTransactions ()}, 1337 );
    }

    //----------------------------------------------------------------//
    redirect ( url ) {
        console.log ( 'REDIRECT:', this.prefixURL ( url ));
        return (<Redirect to = { this.prefixURL ( url )}/>);
    }

    //----------------------------------------------------------------//
    @action
    register ( passwordHash ) {

        this.session = this.makeSession ( true );
        this.passwordHash = passwordHash;
    }

    //----------------------------------------------------------------//
    saveAccount ( accountId, privateKey, publicKey ) {

        this.affirmAccountAndKey ( accountId, 'master', privateKey, publicKey );
    }

    //----------------------------------------------------------------//
    @action
    setAccountInfo ( balance, nonce ) {

        this.accountInfo = {
            balance:    balance || -1,
            nonce:      typeof ( nonce ) === 'number' ? nonce : -1,
        };
    }

    //----------------------------------------------------------------//
    @action
    setNodeInfo ( nodeURL, type, status ) {
        if ( nodeURL in this.nodes ) {
            const info = this.nodes [ nodeURL ];
            if (( info.type !== type ) || ( info.status !== status )) {
                this.nodes [ nodeURL ] = this.makeNodeInfo ( type, status );
            }
        }
    }

    //----------------------------------------------------------------//
    setUser ( userId ) {

        userId = userId || '';

        if ( this.userId !== userId ) {

            console.log ( 'CHANGED USER:', userId );
            this.loadState ( userId );
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
            this.transactions,
            {[ fieldValues.makerAccountName ] : transaction }
        );

        console.log ( 'START TRANSACTION', transactions );

        this.persistValue ( STORE_TRANSACTIONS, transactions );
        this.transactions = transactions;
    }
}
