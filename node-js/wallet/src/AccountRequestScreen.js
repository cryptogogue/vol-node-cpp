/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import { AppStateService }                  from './AppStateService';
import { randomBytes }                      from './util/randomBytes'; // TODO: stop using this
import { Service, useService }              from './Service';
import { SingleColumnContainerView }        from './SingleColumnContainerView'
import * as util                            from './util/util';
import * as crypto                          from './util/crypto';
import _                                    from 'lodash';
import { action, computed, extendObservable, observable, observe }              from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Button, Divider, Dropdown, Form, Header, Icon, Modal, Segment }  from 'semantic-ui-react';

//================================================================//
// AccountReqestForm
//================================================================//
const AccountReqestForm = observer (( props ) => {

    const { appState } = props;

    const [ password, setPassword ]         = useState ( '' );
    const [ seedPhrase, setSeedPhrase ]     = useState ( crypto.generateMnemonic ());

    const createAccountRequest = () => {
        appState.setAccountRequest ( seedPhrase, password );
    }

    const submitEnabled = appState.checkPassword ( password );

    return (
        <div>
            <SingleColumnContainerView title = 'Create Account Request'>

                <Form size = "large" onSubmit = {() => { createAccountRequest ()}}>
                    <Segment stacked>
                        <Segment stacked onClick = {() => { setSeedPhrase ( crypto.generateMnemonic ())}}>
                            { seedPhrase }
                        </Segment>
                        <Form.Input
                            fluid
                            icon = "lock"
                            iconPosition = "left"
                            placeholder = "Wallet Password"
                            type = "password"
                            value = { password }
                            onChange = {( event ) => { setPassword ( event.target.value )}}
                        />
                        <Button color = "teal" fluid size = "large" disabled = { !submitEnabled }>
                            Create Account Request
                        </Button>
                    </Segment>
                </Form>

            </SingleColumnContainerView>
        </div>
    );
});

//================================================================//
// PendingAccountView
//================================================================//
const PendingAccountView = observer (( props ) => {

    const { appState, pending } = props;

    const createAccountRequest = () => {
        appState.deleteAccountRequest ( pending.requestID );
    }

    return (

        <div>
            <SingleColumnContainerView title = 'Pending Account Request'>

                <Form size = "large" onSubmit = {() => { createAccountRequest ()}}>
                    <Segment stacked>
                        <Segment stacked style = {{ wordWrap: 'break-word' }}>
                            { pending.encoded }
                        </Segment>
                        <Button color = "red" fluid size = "large">
                            Delete
                        </Button>
                    </Segment>
                </Form>

            </SingleColumnContainerView>
        </div>
    );
});

//================================================================//
// AccountRequestScreen
//================================================================//
export const AccountRequestScreen = observer (( props ) => {

    const appState = useService (() => new AppStateService ( util.getUserId ( props )));

    const pending = _.values ( appState.pendingAccounts )[ 0 ] || false;

    return (
        <Choose>
            <When condition = { pending !== false }>
                <PendingAccountView appState = { appState } pending = { pending }/>
            </When>
            <Otherwise>
                <AccountReqestForm appState = { appState }/>
            </Otherwise>
        </Choose>
    );
});
