/* eslint-disable no-whitespace-before-property */

import { TransactionListView }      from './TransactionListView';
import React, { useState }          from 'react';
import { observer }                 from 'mobx-react';
import { Button, Divider, Form, Header, Icon, Label, Modal, Segment } from 'semantic-ui-react';

//================================================================//
// StagedTransactionsModal
//================================================================//
export const StagedTransactionsModal = observer (( props ) => {

    const { appState, trigger } = props;

    const [ password, setPassword ] = useState ( '' );

    let onClickSubmit = () => { appState.submitTransactions ( password )};
    let onClickClear = () => { appState.clearStagedTransactions ()};

    const submitEnabled = ( appState.checkPassword ( password ) && appState.canSubmitTransactions );

    const stagedTransactions = appState.stagedTransactions.length;

    return (

        <Choose>
            <When condition = { stagedTransactions === 0 }>
                { trigger }
            </When>

            <Otherwise>
                <Modal
                    style={{ height:'auto' }}
                    size = 'small'
                    
                    trigger = { trigger }
                >
                    <Modal.Header>Staged Transactions</Modal.Header>

                    <Modal.Content>
                        <TransactionListView transactions = { appState.stagedTransactions }/>
                        <Form.Input
                            fluid
                            icon = "lock"
                            iconPosition = "left"
                            placeholder = "Wallet Password"
                            type = "password"
                            value = { password }
                            onChange = {( event ) => { setPassword ( event.target.value )}}
                        />
                    </Modal.Content>

                    <Modal.Actions>
                        <Button negative onClick = { onClickClear }>Clear</Button>
                        <Button
                            positive
                            disabled = { !submitEnabled }
                            onClick = { onClickSubmit }>Submit
                        </Button>
                    </Modal.Actions>
                </Modal>
            </Otherwise>
        </Choose>
    );
});
