/* eslint-disable no-whitespace-before-property */

import StripeCheckoutForm       from './StripeCheckoutForm';
import * as storage             from './util/storage';
import React, { Component }     from 'react';
import { Button, Form, Grid, Header, Segment } from 'semantic-ui-react';
import { Elements, StripeProvider } from 'react-stripe-elements';

const STRIPE_SETTINGS   = 'vol_stripe_settings';

//================================================================//
// DebugStripeScreen
//================================================================//
class DebugStripeScreen extends Component {

    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        const settings = storage.getItem ( STRIPE_SETTINGS );

        let state = {
            stripeURL:          ( settings && settings.stripeURL ) || '',
            stripePublicKey:    ( settings && settings.stripePublicKey ) || '',
            isReady:            false,
        };

        this.state = state;
    }

    //----------------------------------------------------------------//
    handleChange ( event ) {

        this.setState ({[ event.target.name ]: event.target.value });
    }

    //----------------------------------------------------------------//
    handleSubmit () {

        let settings = {
            stripeURL: this.state.stripeURL,
            stripePublicKey: this.state.stripePublicKey,
        };
        storage.setItem ( STRIPE_SETTINGS, settings );

        this.setState ({ isReady: true });
    }

    //----------------------------------------------------------------//
    render () {

        const { isReady, stripeURL, stripePublicKey } = this.state;
        const isEnabled = ( stripeURL.length > 0 ) && ( stripePublicKey.length > 0 );

        let onChange        = ( event ) => { this.handleChange ( event )};
        let onSubmit        = () => { this.handleSubmit ()};

        let stripeForm;

        if ( isReady ) {
            stripeForm = (
                <StripeProvider apiKey = { stripePublicKey }>
                    <div className = "example">
                        <Elements>
                            <StripeCheckoutForm stripeURL = { stripeURL }/>
                        </Elements>
                    </div>
                </StripeProvider>
            );
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
                            Test Stripe Payment
                        </Header>
                        <Form size = "large" onSubmit = { onSubmit }>
                            <Segment stacked>
                                <Form.Input
                                    fluid
                                    icon = "lock"
                                    iconPosition = "left"
                                    placeholder = "Stripe URL"
                                    name = "stripeURL"
                                    value = { this.state.stripeURL }
                                    onChange = { onChange }
                                />
                                <Form.Input
                                    fluid
                                    icon = "lock"
                                    iconPosition = "left"
                                    placeholder = "Stripe public key"
                                    name = "stripePublicKey"
                                    value = { this.state.stripePublicKey }
                                    onChange = { onChange }
                                />
                                <Button color = "red" fluid size = "large" disabled = { !isEnabled }>
                                    Create form
                                </Button>
                            </Segment>
                        </Form>

                        { stripeForm }

                    </Grid.Column>
                </Grid>
            </div>
        );
    }
}
export default DebugStripeScreen;