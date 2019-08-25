/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import { Service, useService }                                                      from '../Service';
import * as crypto                                                                  from '../util/crypto';
import * as util                                                                    from '../util/util';
import CryptoJS                                                                     from 'crypto-js';
import { action, computed, extendObservable, observable, observe, runInAction }     from 'mobx';
import { observer }                                                                 from 'mobx-react';
import React, { useState }                                                          from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment }      from 'semantic-ui-react';

//================================================================//
// DebugAESScreenController
//================================================================//
class DebugAESScreenController extends Service {

    @observable message         = '';
    @observable password        = '';
    @observable ciphertext      = '';
    @observable plaintext       = '';

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();
    }

    //----------------------------------------------------------------//
    @action
    setMessage ( message ) {
        this.message = message;
        this.update ();
    }

    //----------------------------------------------------------------//
    @action
    setPassword ( password ) {
        this.password = password;
        this.update ();
    }

    //----------------------------------------------------------------//
    @action
    update () {

        this.ciphertext     = CryptoJS.AES.encrypt ( this.message, this.password ).toString ();
        this.plaintext      = CryptoJS.AES.decrypt ( this.ciphertext, this.password ).toString ( CryptoJS.enc.Utf8 );
    }
}

//================================================================//
// DebugAESScreen
//================================================================//
const DebugAESScreen = observer (( props ) => {

    const controller    = useService (() => new DebugAESScreenController ());

    return (
    
        <div className='login-form'>
            {/*
                The styles below are necessary for the correct render of this form.
                You can do same with CSS, the main idea is that all the elements up to the `Grid`
                below must have a height of 100%.
            */}
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
                    Test Mnemonic Phrase or Private Key
                </Header>
                <Form size = "large">
                    <Segment stacked>
                        <Form.TextArea
                            rows = { 8 }
                            placeholder = "Message"
                            name = "message"
                            value = { controller.message }
                            onChange = {( event ) => { controller.setMessage ( event.target.value )}}
                        />
                        <input
                            placeholder = "Password"
                            type = "text"
                            value = { controller.password }
                            onChange = {( event ) => { controller.setPassword ( event.target.value )}}
                        />
                        <div className = "ui hidden divider" ></div>
                        <Segment raised style = {{ wordWrap: 'break-word' }}>{ controller.ciphertext }</Segment>
                        <Segment raised style = {{ wordWrap: 'break-word' }}>{ controller.plaintext }</Segment>
                    </Segment>
                </Form>
                </Grid.Column>
            </Grid>
        </div>
    );
});

export default DebugAESScreen;