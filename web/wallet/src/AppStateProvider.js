/* eslint-disable no-whitespace-before-property */

import * as storage             from './utils/storage';
import React, { Component }     from 'react';

const STORE_ACCOUNTS        = 'vol_accounts';
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
    constructor ( props ) {
        super ( props );

        this.state = this.loadState ();
    }

    //----------------------------------------------------------------//
    // DELETE account from local storage
    deleteStorage = () => {
        storage.clear ();
        this.setState ( this.makeClearState ());
    }

    //----------------------------------------------------------------//
    hasUser = () => {
        return ( this.state.user.passwordHash.length > 0 );
    }

    //----------------------------------------------------------------//
    isLoggedIn = () => {
        return ( this.state.session.isLoggedIn === true );
    }

    //----------------------------------------------------------------//
    loadState = () => {

        let state = this.makeClearState ();

        state.user.passwordHash = storage.getItem ( STORE_PASSWORD_HASH ) || state.user.passwordHash;
        state.accounts = storage.getItem ( STORE_ACCOUNTS ) || state.accounts;
        state.session = storage.getItem ( STORE_SESSION ) || state.session;

        return state;
    }

    //----------------------------------------------------------------//
    // LOGIN to the app
    // TODO: Pass clear errors/error messages
    login = ( status ) => {

        const session = {
            isLoggedIn : status,
        }

        storage.setItem ( STORE_SESSION, session );
        this.setState ({ session : session });
    }

    //----------------------------------------------------------------//
    // CLEAR App State (reset to initial state)
    makeClearState = () => {

        let state = {
            accounts : {},
            session : {
                isLoggedIn : false,
            },
            user : {
                passwordHash : '',
                confirmPassword : '',
                password : ''
            }
        };

        return state;
    }

    //----------------------------------------------------------------//
    register = ( passwordHash ) => {

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
    saveAccount = ( accountId, privateKey, publicKey ) => {

        let account = {
            keys : {
                master : {
                    privateKey : privateKey, // TODO: encrypt this with password
                    publicKey : publicKey
                    
                }
            }
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