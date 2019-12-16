/* eslint-disable no-whitespace-before-property */

import { AppStateService }                  from './AppStateService';
import { assert, excel, hooks, Service, SingleColumnContainerView, util } from 'fgc';
import { action, computed, extendObservable, observable, observe } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Button, Divider, Dropdown, Form, Icon, Modal, Segment } from 'semantic-ui-react';

import * as bcrypt              from 'bcryptjs';

//================================================================//
// LoginScreen
//================================================================//
export const LoginScreen = observer (( props ) => {

    const [ errorMessage, setErrorMessage ] = useState ( '' );
    const [ password, setPassword ] = useState ( '' );

    const appState = hooks.useFinalizable (() => new AppStateService ( util.getUserId ( props )));

    if ( !appState.hasUser ()) return appState.redirect ( '/' );
    if ( appState.isLoggedIn ()) return appState.redirect ( '/accounts' );

    const onChange      = ( event ) => { setPassword ( event.target.value )};

    const onSubmit = () => {
        
        if ( appState.checkPassword ( password )) {
            appState.login ( true );
        }
        else {
            setErrorMessage ( 'Invalid password.' );
        }
    }

    const isEnabled = password.length > 0;

    return (
        <SingleColumnContainerView title = 'Login to your wallet'>
            <Form size = "large" onSubmit = { onSubmit }>
                <Segment stacked>
                    <Form.Input
                        fluid
                        icon = "lock"
                        iconPosition = "left"
                        placeholder = "Password"
                        type = "password"
                        name = "password"
                        value = { password }
                        onChange = { onChange }
                        error = {( errorMessage.length > 0 ) ? true : false}
                    />
                    {( errorMessage.length > 0 ) && <span>{ errorMessage }</span>}
                    <Button color = "teal" fluid size = "large" disabled = { !isEnabled }>
                        Login
                    </Button>
                </Segment>
            </Form>
        </SingleColumnContainerView>
    );
});
