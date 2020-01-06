/* eslint-disable no-whitespace-before-property */

import { AppStateService }                  from './AppStateService';
import { PasswordInputField }               from './PasswordInputField';
import { assert, hooks }                    from 'fgc';
import { action, computed, extendObservable, observable, observe } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import * as UI                              from 'semantic-ui-react';

//================================================================//
// LoginForm
//================================================================//
export const LoginForm = observer (( props ) => {

    const { appState } = props;
    const [ password, setPassword ] = useState ( '' );

    const isEnabled = ( password.length > 0 );

    const onSubmit = () => {
        appState.login ( password );
    }

    return (
        <React.Fragment>

            <UI.Header as="h2" color="teal" textAlign="center">
                { 'Log in to your wallet.' }
            </UI.Header>

            <UI.Form size = "large">
                <UI.Segment stacked>
                    <PasswordInputField
                        appState = { appState }
                        setPassword = { setPassword }
                    />
                    <UI.Button
                        fluid
                        color = "teal"
                        size = "large"
                        disabled = { !isEnabled }
                        onClick = { onSubmit }
                    >
                        Login
                    </UI.Button>
                </UI.Segment>
            </UI.Form>

        </React.Fragment>
    );
});
