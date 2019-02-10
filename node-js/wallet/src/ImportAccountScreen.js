/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

// https://www.npmjs.com/package/js-crypto-utils

import { withAppStateAndUser }  from './AppStateProvider';
import BaseComponent            from './BaseComponent';
import * as crypto              from './utils/crypto';
import React                    from 'react';
import { Button, Form, Grid, Header, Segment } from 'semantic-ui-react';

const STATUS_WAITING_FOR_INPUT          = 0;
const STATUS_VERIFYING_KEY              = 1;
const STATUS_DONE                       = 2;

//================================================================//
// ImportAccountScreen
//================================================================//
class ImportAccountScreen extends BaseComponent {

    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        this.state = {
            accountId: '',
            errorMessage: '',
            phraseOrKey: '',
            searchCount: 0,
            status: STATUS_WAITING_FOR_INPUT,
        };
    }

    //----------------------------------------------------------------//
    handleChange ( event ) {

        this.setState ({[ event.target.name ]: event.target.value });
    }

    //----------------------------------------------------------------//
    async handleSubmit () {

        try {
            const key = await crypto.loadKeyAsync ( this.state.phraseOrKey );
            this.verifyKey ( key );
        }
        catch ( error ) {

            console.log ( error );
        }
    }

    //----------------------------------------------------------------//
    render () {

        const { appState } = this.props;
        const { minerURLs } = appState;
        const hasMiners = minerURLs.size > 0;

        if ( this.state.status === STATUS_DONE ) return this.redirect ( '/accounts/' + this.state.accountId );

        
        const { errorMessage, phraseOrKey } = this.state;

        const inputEnabled = hasMiners;
        const submitEnabled = inputEnabled && ( phraseOrKey.length > 0 );

        let onChange    = ( event ) => { this.handleChange ( event )};
        let onSubmit    = () => { this.handleSubmit ()};

        let warning;
        if ( !hasMiners ) {
            warning = (
                <Header as="h4" color="red" textAlign="center">
                    No mining nodes found. Offline or none listed.
                </Header>
            );
        }

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
                        Import your account
                    </Header>
                    { warning }
                    <Form size = "large" onSubmit = { onSubmit }>
                        <Segment stacked>
                            <Form.TextArea
                                placeholder = "Mnemonic Phrase or Private Key"
                                name = "phraseOrKey"
                                value = { this.state.phraseOrKey }
                                onChange = { onChange }
                                error = {( errorMessage.length > 0 ) ? true : false }
                                disabled = { !inputEnabled }
                            />
                            {( errorMessage.length > 0 ) && <span>{ errorMessage }</span>}
                            <Button color = "teal" fluid size = "large" disabled = { !submitEnabled }>
                                Login
                            </Button>
                        </Segment>
                    </Form>
                    </Grid.Column>
                </Grid>
            </div>
        );
    }

    //----------------------------------------------------------------//
    async verifyKey ( key ) {

        const { appState } = this.props;

        const publicKey = key.getPublicHex ();
        console.log ( 'PUBLIC_KEY', publicKey );

        // check to see if we already have the key, in which case early out
        let accountId = appState.findAccountIdByPublicKey ( publicKey );

        if ( accountId ) {

            console.log ( 'ACCOUNT KEY ALREADY EXISTS' );

            this.setState ({
                accountId: accountId,
                status: STATUS_DONE,
            });
            return;
        }

        const keyID = key.getKeyID ();
        console.log ( 'KEY_ID', keyID );

        this.setState ({
            status: STATUS_VERIFYING_KEY,
            searchCount: 0,
        })
 
        let keyName = false;
        let searchCount = 0;

        let verify = async ( url ) => {

            try {
                const data = await this.revocableFetchJSON ( url + '/keys/' + keyID );

                const keyInfo = data && data.keyInfo;

                if ( keyInfo ) {
                    accountId = keyInfo.accountName;
                    keyName = keyInfo.keyName;
                }
            }
            catch ( error ) {
                console.log ( error );
            }
            this.setState ({ searchCount: searchCount++ });
        }

        appState.minerURLs.forEach ( async ( url ) => {

            await verify ( url );

            if ( accountId ) {

                const privateKey = key.getPrivateHex ();
                appState.affirmAccountAndKey ( accountId, keyName, privateKey, publicKey ); // TODO: should affirm the account and key

                this.setState ({
                    accountId: accountId,
                    status: STATUS_DONE,
                });
                return;
            }
        });
        
        this.setState ({
            errorMessage: 'Account not found.',
            status: STATUS_WAITING_FOR_INPUT,
        });
    }
}

export default withAppStateAndUser ( ImportAccountScreen );
