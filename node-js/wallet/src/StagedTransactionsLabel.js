/* eslint-disable no-whitespace-before-property */

import { PasswordInputField }       from './PasswordInputField';
import { TransactionListView }      from './TransactionListView';
import React, { useState }          from 'react';
import { observer }                 from 'mobx-react';
import * as UI                      from 'semantic-ui-react';

//================================================================//
// StagedTransactionsModalBody
//================================================================//
const StagedTransactionsModalBody = observer (( props ) => {

    const { appState, open, onClose } = props;

    const [ password, setPassword ] = useState ( '' );

    let onClickSubmit = () => {
        appState.submitTransactions ( password );
        onClose ();
    };
    
    let onClickClear = () => {
        appState.clearStagedTransactions ();
        onClose ();
    };

    const submitEnabled = ( appState.checkPassword ( password ) && appState.canSubmitTransactions );

    const stagedTransactions = appState.stagedTransactions.length;

    return (
        <UI.Modal
            size = 'small'
            closeIcon
            onClose = {() => { onClose ()}}
            open = { open }
        >
            <UI.Modal.Header>Staged Transactions</UI.Modal.Header>

            <UI.Modal.Content>
                <TransactionListView transactions = { appState.stagedTransactions }/>
                <UI.Form>
                    <PasswordInputField
                        appState = { appState }
                        setPassword = { setPassword }
                    />
                </UI.Form>
            </UI.Modal.Content>

            <UI.Modal.Actions>

                <UI.Button
                    negative
                    disabled = { !submitEnabled }
                    onClick = { onClickClear }
                >
                    Clear All
                </UI.Button>

                <UI.Button
                    positive
                    disabled = { !submitEnabled }
                    onClick = { onClickSubmit }
                >
                    Submit
                </UI.Button>

            </UI.Modal.Actions>
        </UI.Modal>
    );
});

//================================================================//
// StagedTransactionsModalLabel
//================================================================//
export const StagedTransactionsLabel = observer (( props ) => {

    const { appState } = props;
    const [ open, setOpen ] = useState ( false );
    const [ counter, setCounter ] = useState ( 0 );

    const onClose = () => {
        setOpen ( false );
        setCounter ( counter + 1 );
    }

    const stagedTransactions = appState.stagedTransactions.length;

    return (
        <React.Fragment>

            <div key = { counter }>
                <StagedTransactionsModalBody
                    appState = { appState }
                    open = { open }
                    onClose = { onClose }
                />
            </div>

            <UI.Label
                color = { stagedTransactions > 0 ? 'green' : 'grey' }
                disabled = { stagedTransactions === 0 }
                onClick = {() => { stagedTransactions && setOpen ( true )}}
            >
                <UI.Icon name = 'cloud upload'/>
                { stagedTransactions }
            </UI.Label>

        </React.Fragment>
    );
});
