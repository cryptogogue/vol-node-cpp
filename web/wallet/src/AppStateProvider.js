/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import * as storage             from './utils/storage';
import React, { Component }     from 'react';

const STORE_ACCOUNTS        = 'vol_accounts';
const STORE_NODES           = 'vol_nodes';
const STORE_PASSWORD_HASH   = 'vol_password_hash';
const STORE_SESSION         = 'vol_session';

const TheContext = React.createContext ();
const AppStateConsumer = TheContext.Consumer;

//----------------------------------------------------------------//
function withAppState ( WrappedComponent ) {

    return class extends React.Component {

        render () {
            return (
                <AppStateConsumer>
                    {({appState}) => (
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
class AppStateProvider extends Component {

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

        this.state = this.loadState ();

        // this.discoveryTimer = setTimeout ( this.discoverNetwork, 0 );
    }

    //----------------------------------------------------------------//
    // DELETE account from local storage
    deleteStorage () {
        storage.clear ();
        this.setState ( this.makeClearState ());
    }

    //----------------------------------------------------------------//
    // discoverNetwork () {

    //     console.log ( 'DISCOVER NETWORK' );

    //     const { nodes } = this.state;

    //     let count = 0;
    //     let top = nodes.length;

    //     let finish = () => {
    //         if ( count >= top ) {
    //             this.discoveryTimer = setTimeout ( this.discoverNetwork, 1000 );
    //         }
    //     }

    //     for ( let i in nodes ) {

    //         const nodeURL = nodes [ i ];

    //         console.log ( 'NODE URL:', nodeURL );

    //         fetch ( nodeURL )
    //         .then (( response ) => {

    //             return response.json();
    //         })
    //         .then (( data ) => {

    //             console.log ( data );
    //         }) 
    //         .catch (( error ) => {

    //             console.log ( error );
    //         })
    //         .finally (() => {
    //             count++;
    //             finish ();
    //         });
    //     }

    //     finish ()
    // }

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
            accounts : {},
            session : {
                isLoggedIn : false,
            },
            nodes : [],
            user : {
                passwordHash : '',
                confirmPassword : '',
                password : ''
            }
        };

        return state;
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