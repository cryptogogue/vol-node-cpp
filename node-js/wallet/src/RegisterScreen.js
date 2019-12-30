/* eslint-disable no-whitespace-before-property */

import { AppStateService }                  from './AppStateService';
import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';
import { action, computed, extendObservable, observable, observe } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Redirect }                         from 'react-router';
import { Button, Divider, Dropdown, Form, Icon, Modal, Segment }  from 'semantic-ui-react';

import * as bcrypt              from 'bcryptjs';

//================================================================//
// RegisterScreenController
//================================================================//
class RegisterScreenController {

    //----------------------------------------------------------------//
    constructor ( appState ) {

        this.appState   = appState;

        extendObservable ( this, {
            password: '',
            confirmPassword: '',
            errorMessage: '',
        });
    }

    //----------------------------------------------------------------//
    finalize () {
    }

    //----------------------------------------------------------------//
    @action
    handleChange ( event ) {

        this [ event.target.name ] = event.target.value;
    }

    //----------------------------------------------------------------//
    handleSubmit () {

        // Hash password with salt
        let passwordHash = bcrypt.hashSync ( this.password, 10 );

        // Check to see if hash exists and commit the App State to Transactioned Storage
        if ( passwordHash.length > 0 ) {
            this.appState.register ( passwordHash );
        }
        else {
            console.log ( "Failed to hash password." )
        }
    }

    //----------------------------------------------------------------//
    @computed get
    readyToSubmit () {
        return (( this.password.length > 0 ) && ( this.password === this.confirmPassword ));
    }
}
    
//================================================================//
// RegisterScreen
//================================================================//
export const RegisterScreen = observer (( props ) => {
    
    const appState = hooks.useFinalizable (() => new AppStateService ());
    const controller = hooks.useFinalizable (() => new RegisterScreenController ( appState ));

    if ( appState.hasUser ()) {
        const to = appState.isLoggedIn () ? '/accounts' : '/login';
        return (<Redirect to = { to }/>);
    }

    let onChange        = ( event ) => { controller.handleChange ( event )};
    let onSubmit        = () => { controller.handleSubmit ()};

    const isEnabled = controller.readyToSubmit;

    return (
        <SingleColumnContainerView title = 'Choose a password for your wallet.'>

            <Form size = "large" onSubmit = { onSubmit }>
                <Segment stacked>
                    <Form.Input
                        fluid
                        icon = "lock"
                        iconPosition = "left"
                        placeholder = "Password"
                        type = "password"
                        name = "password"
                        value = { controller.password }
                        onChange = { onChange }
                    />
                    <Form.Input
                        fluid
                        icon = "lock"
                        iconPosition = "left"
                        placeholder = "Confirm password"
                        type = "password"
                        name = "confirmPassword"
                        value = { controller.confirmPassword }
                        onChange = { onChange }
                    />
                    { controller.errorMessage && <span>{ controller.errorMessage }</span>}
                    <Button color = "red" fluid size = "large" disabled = { !isEnabled }>
                        Create Password
                    </Button>
                </Segment>
            </Form>

        </SingleColumnContainerView>
    );
});
