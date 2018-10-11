/* eslint-disable no-whitespace-before-property */

import { withAppState }         from './AppStateProvider';
import * as bcrypt              from 'bcryptjs';
import React, { Component }     from 'react';
import { Redirect }             from 'react-router-dom';
import { Button, Form, Grid, Header, Message, Segment } from 'semantic-ui-react';

//================================================================//
// RegisterScreen
//================================================================//
class RegisterScreen extends Component {

    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        this.state = {
            password: '',
            confirmPassword: '',
        };

        this.handleChange = this.handleChange.bind ( this );
        this.handleSubmit = this.handleSubmit.bind ( this );
    }

    //----------------------------------------------------------------//
    handleChange ( event ) {

        this.setState ({[ event.target.name ]: event.target.value });
    }

    //----------------------------------------------------------------//
    handleSubmit ( event ) {

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
        const isEnabled = password.length > 0 && ( password === confirmPassword );
        
        if ( appState.hasUser ()) {
            const to = appState.isLoggedIn () ? '/accounts' : '/login';
            return (<Redirect to = { to }/>);
        }

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
                            Register your account
                        </Header>
                        <Form size = "large" onSubmit = { this.handleSubmit }>
                            <Segment stacked>
                                <Form.Input
                                    fluid
                                    icon = "lock"
                                    iconPosition = "left"
                                    placeholder = "Password"
                                    type = "password"
                                    name = "password"
                                    value = { this.state.password }
                                    onChange = { this.handleChange }
                                />
                                <Form.Input
                                    fluid
                                    icon = "lock"
                                    iconPosition = "left"
                                    placeholder = "Confirm password"
                                    type = "password"
                                    name = "confirmPassword"
                                    value = { this.state.confirmPassword }
                                    onChange = { this.handleChange }
                                />
                                { errorMessage && <span>{ errorMessage }</span>}
                                <Button color = "red" fluid size = "large" disabled = { !isEnabled }>
                                    Create Account
                                </Button>
                            </Segment>
                        </Form>
                        <Message>
                            Already have an account? <a>Import</a>
                        </Message>
                    </Grid.Column>
                </Grid>
            </div>
        );
    }
}

export default withAppState ( RegisterScreen );