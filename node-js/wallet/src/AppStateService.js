/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import * as crypto          from './util/crypto';
import { randomBytes }      from './util/randomBytes'; // TODO: stop using this
import * as storage         from './util/storage';
import { Service }          from './Service';
import * as bcrypt          from 'bcryptjs';
import _                    from 'lodash';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import React                from 'react';
import { Redirect }         from 'react-router';

const STORE_ACCOUNTS            = '.vol_accounts';
const STORE_NODE                = '.vol_node';
const STORE_NODES               = '.vol_nodes';
const STORE_PASSWORD_HASH       = '.vol_password_hash';
const STORE_PENDING_ACCOUNTS    = '.vol_pending_accounts';
const STORE_SESSION             = '.vol_session';

export const NODE_TYPE = {
    UNKNOWN:    'UNKNOWN',
    MINING:     'MINING',
    MARKET:     'MARKET',
};

export const NODE_STATUS = {
    UNKNOWN:    'UNKNOWN',
    ONLINE:     'ONLINE',
    OFFLINE:    'OFFLINE',
};

//================================================================//
// AppStateService
//================================================================//
export class AppStateService extends Service {

    @observable userId;
    @observable accountId;
    @observable accountInfo;
    @observable nextTransactionCost;

    // persisted
    @observable accounts;
    @observable node;
    @observable nodes;
    @observable passwordHash;
    @observable pendingAccounts;
    @observable session;
    @observable transactions;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    // computed

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
            if (( info.type === NODE_TYPE.MARKET ) && ( info.status === NODE_STATUS.ONLINE )) {
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
    @computed get
    assetsUtilized () {

        let assetsUtilized = [];

        const pendingTransactions = this.pendingTransactions;
        for ( let i in pendingTransactions ) {
            assetsUtilized = assetsUtilized.concat ( pendingTransactions [ i ].assets );
        }

        const stagedTransactions = this.stagedTransactions;
        for ( let i in stagedTransactions ) {
            assetsUtilized = assetsUtilized.concat ( stagedTransactions [ i ].assets );
        }
        return assetsUtilized;
    }

    //----------------------------------------------------------------//
    @computed get
    balance () {

        let cost = 0;

        const pendingTransactions = this.pendingTransactions;
        for ( let i in pendingTransactions ) {
            cost += pendingTransactions [ i ].cost;
        }

        const stagedTransactions = this.stagedTransactions;
        for ( let i in stagedTransactions ) {
            cost += stagedTransactions [ i ].cost;
        }

        return this.accountInfo.balance - cost - this.nextTransactionCost;
    }

    //----------------------------------------------------------------//
    @computed get
    canSubmitTransactions () {

        if ( this.nextNonce < 0 ) return false;
        if ( this.node.length === 0 ) return false;
        if ( this.account.stagedTransactions.length === 0 ) return false;

        return this.hasActiveMiningNode;
    }

    //----------------------------------------------------------------//
    @computed get
    hasAccount () {
        return (( this.accountId.length > 0 ) && this.account );
    }

    //----------------------------------------------------------------//
    @computed get
    hasAccountInfo () {
        return ( this.accountInfo.nonce >= 0 );
    }

    //----------------------------------------------------------------//
    @computed get
    hasActiveMiningNode () {

        const nodeInfo = this.nodeInfo;

        if ( nodeInfo.type !== NODE_TYPE.MINING ) return false;
        if ( nodeInfo.status !== NODE_STATUS.ONLINE ) return false;

        return true;
    }

    //----------------------------------------------------------------//
    @computed get
    nextNonce () {

        if ( this.nonce < 0 ) return -1;

        const pendingTransactions = this.pendingTransactions;
        const pendingTop = pendingTransactions.length;

        return pendingTop > 0 ? pendingTransactions [ pendingTop - 1 ].nonce + 1 : this.nonce;
    }

    //----------------------------------------------------------------//
    @computed get
    nodeInfo () {
        return this.getNodeInfo ();
    }

    //----------------------------------------------------------------//
    @computed get
    nonce () {
        return this.accountInfo.nonce;
    }

    //----------------------------------------------------------------//
    @computed get
    pendingTransactions () {
        return this.account.pendingTransactions || [];
    }

    //----------------------------------------------------------------//
    @computed get
    stagedTransactions () {
        return this.account.stagedTransactions || [];
    }

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    // methods

    //----------------------------------------------------------------//
    @action
    affirmAccountAndKey ( accountId, keyName, privateKeyHex, publicKeyHex, password ) {

        if ( !this.checkPassword ( password )) throw new Error ( 'Invalid wallet password' );

        let accounts = this.accounts;

        let account = accounts [ accountId ] || {
            keys: {},
            pendingTransactions: [],
            stagedTransactions: [],
        };

        let key = account.keys [ keyName ] || {};

        key.privateKeyHexCiphertext = crypto.aesPlainToCipher ( privateKeyHex, password );
        key.publicKeyHex = publicKeyHex;

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

        this.observeMember ( 'accounts',            () => { this.persistValue ( STORE_ACCOUNTS, this.accounts )});
        this.observeMember ( 'node',                () => { this.persistValue ( STORE_NODE, this.node )});
        this.observeMember ( 'nodes',               () => { this.persistValue ( STORE_NODES, this.nodes )});
        this.observeMember ( 'passwordHash',        () => { this.persistValue ( STORE_PASSWORD_HASH, this.passwordHash )});
        this.observeMember ( 'pendingAccounts',     () => { this.persistValue ( STORE_PENDING_ACCOUNTS, this.pendingAccounts )});
        this.observeMember ( 'session',             () => { this.persistValue ( STORE_SESSION, this.session )});
    }

    //----------------------------------------------------------------//
    checkPassword ( password ) {
        const passwordHash = ( this.passwordHash ) || '';
        return (( passwordHash.length > 0 ) && bcrypt.compareSync ( password, passwordHash ));
    }

    //----------------------------------------------------------------//
    // CLEAR App State (reset to initial state)
    @action
    clearState () {

        this.nextTransactionCost = 0;

        this.accounts           = {};
        this.session            = this.makeSession ( false );
        this.node               = '';
        this.nodes              = {};
        this.passwordHash       = '';
        this.transactions       = [];

        this.setAccountInfo ();
    }

    //----------------------------------------------------------------//
    @action
    clearStagedTransactions () {

        if ( this.hasAccount ) {
            this.account.stagedTransactions = [];
        }
    }

    //----------------------------------------------------------------//
    @action
    confirmTransactions ( nonce ) {

        let pendingTransactions = this.account.pendingTransactions;
        while (( pendingTransactions.length > 0 ) && ( pendingTransactions [ 0 ].nonce <= nonce )) {
            pendingTransactions.shift ();
        }
    }

    //----------------------------------------------------------------//
    constructor ( userId, accountId ) {
        super ();

        this.minerURLs = new Set ();
        this.marketURLs = new Set ();
        this.urlBackoff = {};

        this.loadState ( userId, accountId );
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
    @action
    deleteTransactions () {

        if ( this.hasAccount ) {
            this.account.pendingTransactions = [];
            this.account.stagedTransactions = [];
        }
    }

    //----------------------------------------------------------------//
    deleteUserStorage () {

        storage.removeItem ( this.prefixStoreKey ( STORE_ACCOUNTS ));
        storage.removeItem ( this.prefixStoreKey ( STORE_NODE ));
        storage.removeItem ( this.prefixStoreKey ( STORE_NODES ));
        storage.removeItem ( this.prefixStoreKey ( STORE_PASSWORD_HASH ));
        storage.removeItem ( this.prefixStoreKey ( STORE_PENDING_ACCOUNTS ));
        storage.removeItem ( this.prefixStoreKey ( STORE_SESSION ));

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
    getDefaultAccountKeyName () {
        const defaultKeyName = 'master';
        const accountKeyNames = this.accountKeyNames ;
        if ( accountKeyNames.includes [ defaultKeyName ]) return defaultKeyName;
        return (( accountKeyNames.length > 0 ) && accountKeyNames [ 0 ]) || '';
    }

    //----------------------------------------------------------------//
    getMinerURL () {

        return this.nodes [ 0 ]; // this is a hack for now (until we redo the config)
    }

    //----------------------------------------------------------------//
    getNodeInfo ( url ) {

        return this.nodes [ url || this.node || '' ] || this.makeNodeInfo ();
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
        this.pendingAccounts        = storage.getItem ( userId + STORE_PENDING_ACCOUNTS ) || {};
        this.session                = storage.getItem ( userId + STORE_SESSION ) || this.session;

        for ( let url in this.nodes ) {
            this.nodes [ url ].status = NODE_STATUS.UNKNOWN;
        }

        this.setAccount ( accountId );
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
    @action
    pushTransaction ( transaction ) {

        transaction.body.maker.accountName = this.accountId;
        transaction.body.maker.nonce = -1;

        let account = this.account;

        let memo = {
            type:               transaction.type,
            note:               transaction.note,
            cost:               transaction.getCost (),
            body:               JSON.stringify ( transaction.body, null, 4 ),
            assets:             transaction.assetsUtilized,
        }

        console.log ( 'ASSETS UTILIZED:', memo.assets );

        this.account.stagedTransactions.push ( memo );

        this.setNextTransactionCost ( 0 );
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
    saveAccount ( accountId, privateKeyHex, publicKeyHex, password ) {

        this.affirmAccountAndKey ( accountId, 'master', privateKeyHex, publicKeyHex, password );
    }

    //----------------------------------------------------------------//
    @action
    setAccount ( accountId ) {

        const accountNames = Object.keys ( this.accounts );
        accountId = (( accountId in this.accounts ) && accountId ) || ( accountNames.length && accountNames [ 0 ]) || '';
        if ( this.accountId !== accountId ) {
            this.accountId = accountId;
            this.setAccountInfo ();
        }
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
    setAccountRequest ( accountName, seedPhrase, password ) {

        if ( _.has ( this.pendingAccounts, 'accountName' )) return;

        if ( !this.checkPassword ( password )) throw new Error ( 'Invalid wallet password' );

        const key               = new crypto.mnemonicToKey ( seedPhrase );
        const keyID             = key.getKeyID ();

        const nameHash          = key.sign ( message );

        const salt              = randomBytes ( 16 ).toString ( 'hex' );
        const secret            = `${ keyID }:${ nameHash }:${ salt }`;

        const request = {
            key: {
                type:               'EC_HEX',
                groupName:          'secp256k1',
                publicKey:          key.getPublicHex (),
            },
            signature: {
                hashAlgorithm:      'SHA256',
                digest:             key.hash ( secret ), // use this to reveal the secret
                signature:          key.sign ( secret ),
            },
        }

        const requestJSON   = JSON.stringify ( request );
        const encoded       = Buffer.from ( requestJSON, 'utf8' ).toString ( 'base64' );

        const pendingAccount = {
            accountName:            accountName,
            salt:                   salt,
            keyID:                  keyID,
            seedPhraseCiphertext:   crypto.aesPlainToCipher ( seedPhrase, password ),
            request:                encoded,
            encoding:               'base64',
        }

        this.pendingAccounts [ accountName ] = pendingAccount;
    }

    //----------------------------------------------------------------//
    @action
    setNextTransactionCost ( cost ) {

        this.nextTransactionCost = cost || 0;
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
    @action
    async submitTransactions ( password ) {

        if ( !this.checkPassword ( password )) throw new Error ( 'Invalid wallet password' );

        let stagedTransactions = this.account.stagedTransactions;
        let pendingTransactions = this.account.pendingTransactions;

        try {

            while ( this.canSubmitTransactions ) {

                let memo = stagedTransactions [ 0 ];
                let nonce = this.nextNonce;

                let body = JSON.parse ( memo.body );
                body.maker.nonce = nonce;

                let envelope = {
                    body: JSON.stringify ( body, null, 4 ),
                };

                const hexKey            = this.account.keys [ body.maker.keyName ];
                const privateKeyHex     = crypto.aesCipherToPlain ( hexKey.privateKeyHexCiphertext, password );
                const key               = await crypto.keyFromPrivateHex ( privateKeyHex );

                envelope.signature = {
                    hashAlgorithm:  'SHA256',
                    digest:         key.hash ( envelope.body ),
                    signature:      key.sign ( envelope.body ),
                };
                
                await this.revocableFetch ( this.node + '/transactions', {
                    method :    'POST',
                    headers :   { 'content-type': 'application/json' },
                    body :      JSON.stringify ( envelope, null, 4 ),
                });

                runInAction (() => {
                    stagedTransactions.shift ();
                    pendingTransactions.push ({
                        type:                       memo.type,
                        note:                       memo.note,
                        cost:                       memo.cost,
                        body:                       envelope,
                        nonce:                      nonce,
                        assets:                     memo.assets,
                    });
                });
            }
        }
        catch ( error ) {
             console.log ( 'AN ERROR!', error );
        }
    }
}
