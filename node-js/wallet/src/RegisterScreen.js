/* eslint-disable no-whitespace-before-property */

import { AppStateService }                                                      from './stores/AppStateService';
import { Service, useService }                                                  from './stores/Service';
import * as util                                                                from './util/util';
import { action, computed, extendObservable, observable, observe }              from 'mobx';
import { observer }                                                             from 'mobx-react';
import React, { useState }                                                      from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment }  from 'semantic-ui-react';

import * as bcrypt              from 'bcryptjs';

//================================================================//
// RegisterService
//================================================================//
class RegisterService extends Service {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        this.appState = appState;

        extendObservable ( this, {
            password: '',
            confirmPassword: '',
            errorMessage: '',
        });
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
const RegisterScreen = observer (( props ) => {
    
    const appState = useService (() => new AppStateService ( util.getUserId ( props )));
    const service = useService (() => new RegisterService ( appState ));

    if ( appState.hasUser ()) {
        const to = appState.isLoggedIn () ? '/accounts' : '/login';
        return appState.redirect ( to );
    }

    let onChange        = ( event ) => { service.handleChange ( event )};
    let onSubmit        = () => { service.handleSubmit ()};

    const isEnabled = service.readyToSubmit;

    return (
        <div className = "register-form">
            {/*
            The styles below are necessary for the correct render of this form.
            You can do same with CSS, the main idea is that all the elements up to the `Grid`
            below must have a height of 100%.
            */}
            <style>{`
            body > div,
            body > div > div,
            body > div > div > div.register-form {
                height: 100%;
            }
            `}</style>
            <Grid textAlign = "center" style = {{ height: '100%' }} verticalAlign = "middle">
                <Grid.Column style = {{ maxWidth: 450 }}>
                    <Header as = "h2" color = "red" textAlign = "center">
                        Choose a password for your wallet.
                    </Header>
                    <Form size = "large" onSubmit = { onSubmit }>
                        <Segment stacked>
                            <Form.Input
                                fluid
                                icon = "lock"
                                iconPosition = "left"
                                placeholder = "Password"
                                type = "password"
                                name = "password"
                                value = { service.password }
                                onChange = { onChange }
                            />
                            <Form.Input
                                fluid
                                icon = "lock"
                                iconPosition = "left"
                                placeholder = "Confirm password"
                                type = "password"
                                name = "confirmPassword"
                                value = { service.confirmPassword }
                                onChange = { onChange }
                            />
                            { service.errorMessage && <span>{ service.errorMessage }</span>}
                            <Button color = "red" fluid size = "large" disabled = { !isEnabled }>
                                Create Password
                            </Button>
                        </Segment>
                    </Form>
                </Grid.Column>
            </Grid>
        </div>
    );
});

export default RegisterScreen;