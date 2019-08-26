/* eslint-disable no-whitespace-before-property */

import { TransactionListView }                              from './TransactionListView';
import { Transaction }                                      from './util/Transaction';
import React, { useState }                                  from 'react';
import { observer }                                         from 'mobx-react';
import { Button, Divider, Form, Header, Modal, Segment }    from 'semantic-ui-react';

//================================================================//
// StagedTransactionsView
//================================================================//
export const StagedTransactionsView = observer (( props ) => {

    const appState = props.appState;

    const [ password, setPassword ] = useState ( '' );

    let onClickSubmit = () => { appState.submitTransactions ( password )};
    let onClickClear = () => { appState.clearStagedTransactions ()};

    const submitEnabled = ( appState.checkPassword ( password ) && appState.canSubmitTransactions );

    return (
        <div>
            <Header as = "h2">Staged Transactions</Header>
            <TransactionListView transactions = { appState.stagedTransactions }/>
            <Segment stacked>
                <Form.Input
                    fluid
                    icon = "lock"
                    iconPosition = "left"
                    placeholder = "Wallet Password"
                    type = "password"
                    value = { password }
                    onChange = {( event ) => { setPassword ( event.target.value )}}
                />
                <div className = "ui hidden divider" ></div>
                <Button
                    color = "teal"
                    fluid disabled = { !submitEnabled }
                    onClick = { onClickSubmit }>Submit
                </Button>
                <Button color = "red" fluid onClick = { onClickClear }>Clear</Button>
            </Segment>
        </div>
    );
});
