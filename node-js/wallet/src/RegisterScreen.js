/* eslint-disable no-whitespace-before-property */

import { withAppStateAndUser }  from './AppStateProvider';
import BaseComponent            from './BaseComponent';
import * as bcrypt              from 'bcryptjs';
import React                    from 'react';
import { Button, Form, Grid, Header, Segment } from 'semantic-ui-react';

//================================================================//
// RegisterScreen
//================================================================//
class RegisterScreen extends BaseComponent {

    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        this.state = {
            password: '',
            confirmPassword: '',
        };
    }

    //----------------------------------------------------------------//
    handleChange ( event ) {

        this.setState ({[ event.target.name ]: event.target.value });
    }

    //----------------------------------------------------------------//
    handleSubmit () {

        // Hash password with salt
        let passwordHash = bcrypt.hashSync ( this.state.password, 10 );

        // Check to see if hash exists and commit the App State to Transactioned Storage
        if ( passwordHash.length > 0 ) {
            this.props.appState.register ( passwordHash );
        }
        else {
            console.log ( "Failed to hash password." )
        }
    }
    
    //----------------------------------------------------------------//
    render () {

        const { appState } = this.props;
        const { errorMessage, password, confirmPassword } = this.state;
        const isEnabled = ( password.length > 0 ) && ( password === confirmPassword );
        
        if ( appState.hasUser ()) {
            const to = appState.isLoggedIn () ? '/accounts' : '/login';
            return this.redirect ( to );
        }

        let onChange        = ( event ) => { this.handleChange ( event )};
        let onSubmit        = () => { this.handleSubmit ()};

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
                                    value = { this.state.password }
                                    onChange = { onChange }
                                />
                                <Form.Input
                                    fluid
                                    icon = "lock"
                                    iconPosition = "left"
                                    placeholder = "Confirm password"
                                    type = "password"
                                    name = "confirmPassword"
                                    value = { this.state.confirmPassword }
                                    onChange = { onChange }
                                />
                                { errorMessage && <span>{ errorMessage }</span>}
                                <Button color = "red" fluid size = "large" disabled = { !isEnabled }>
                                    Create Password
                                </Button>
                            </Segment>
                        </Form>
                    </Grid.Column>
                </Grid>
            </div>
        );
    }
}

export default withAppStateAndUser ( RegisterScreen );