/* eslint-disable no-whitespace-before-property */

import { AppStateService }                  from './AppStateService';
import { assert, hooks }                    from 'fgc';
import { action, computed, extendObservable, observable, observe } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Button, Divider, Dropdown, Form, Header, Icon, Modal, Segment } from 'semantic-ui-react';

//================================================================//
// LoginForm
//================================================================//
export const LoginForm = observer (( props ) => {

    const { appState } = props;

    const [ errorMessage, setErrorMessage ] = useState ( '' );
    const [ password, setPassword ] = useState ( '' );

    const onChange = ( event ) => { setPassword ( event.target.value )};

    const onSubmit = () => {
        
        if ( appState.checkPassword ( password )) {
            appState.login ( true );
        }
        else {
            setErrorMessage ( 'Invalid password.' );
        }
    }

    const isEnabled = appState.checkPassword ( password );

    return (
        <React.Fragment>

            <Header as="h2" color="teal" textAlign="center">
                { 'Login to your wallet.' }
            </Header>

            <Form size = "large" onSubmit = { onSubmit }>
                <Segment stacked>
                    <Form.Input
                        fluid
                        icon = { isEnabled ? 'unlock' : 'lock' }
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

        </React.Fragment>
    );
});
