/* eslint-disable no-whitespace-before-property */

import { AppStateService }                                                      from './AppStateService';
import { Service, useService }                                                  from './Service';
import * as util                                                                from './util/util';
import { action, computed, extendObservable, observable, observe }              from 'mobx';
import { observer }                                                             from 'mobx-react';
import React, { useState }                                                      from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment }  from 'semantic-ui-react';

import * as bcrypt              from 'bcryptjs';

//================================================================//
// LoginScreen
//================================================================//
const LoginScreen = observer (( props ) => {

    const [ errorMessage, setErrorMessage ] = useState ( '' );
    const [ password, setPassword ] = useState ( '' );

    const appState = useService (() => new AppStateService ( util.getUserId ( props )));

    if ( !appState.hasUser ()) return appState.redirect ( '/' );
    if ( appState.isLoggedIn ()) return appState.redirect ( '/accounts' );

    const onChange      = ( event ) => { setPassword ( event.target.value )};

    const onSubmit = () => {

        const passwordHash = ( appState.passwordHash ) || '';

        if (( passwordHash.length > 0 ) && bcrypt.compareSync ( password, passwordHash )) {
            appState.login ( true );
        }
        else {
            setErrorMessage ( 'Invalid password.' );
        }
    }

    const isEnabled = password.length > 0;

    return (
    
        <div className='login-form'>
            <style>{`
                body > div,
                body > div > div,
                body > div > div > div.login-form {
                    height: 100%;
                }
            `}</style>
            <Grid textAlign = "center" style = {{ height: '100%' }} verticalAlign = "middle">
                <Grid.Column style={{ maxWidth: 450 }}>
                <Header as="h2" color="teal" textAlign="center">
                    Login to your wallet.
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
                </Grid.Column>
            </Grid>
        </div>
    );
});

export default LoginScreen;