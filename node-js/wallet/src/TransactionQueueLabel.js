/* eslint-disable no-whitespace-before-property */

import { PasswordInputField }       from './PasswordInputField';
import { TransactionListView }      from './TransactionListView';
import React, { useState }          from 'react';
import { observer }                 from 'mobx-react';
import * as UI                      from 'semantic-ui-react';

//================================================================//
// TransactionQueueModalBody
//================================================================//
const TransactionQueueModalBody = observer (( props ) => {

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

    const allTransactions = appState.pendingTransactions.concat ( appState.stagedTransactions );

    const passwordIsValid = appState.checkPassword ( password );
    const clearEnabled = ( passwordIsValid && ( appState.stagedTransactions.length > 0 ));
    const submitEnabled = ( passwordIsValid && appState.canSubmitTransactions );

    return (
        <UI.Modal
            size = 'small'
            closeIcon
            onClose = {() => { onClose ()}}
            open = { open }
        >
            <UI.Modal.Header>Staged Transactions</UI.Modal.Header>

            <UI.Modal.Content>
                <TransactionListView transactions = { allTransactions }/>
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
                    disabled = { !clearEnabled }
                    onClick = { onClickClear }
                >
                    Clear
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
// TransactionQueueLabel
//================================================================//
export const TransactionQueueLabel = observer (( props ) => {

    const { appState } = props;
    const [ open, setOpen ] = useState ( false );
    const [ counter, setCounter ] = useState ( 0 );

    const onClose = () => {
        setOpen ( false );
        setCounter ( counter + 1 );
    }

    const stagedCount       = appState.stagedTransactions.length;
    const pendingCount      = appState.pendingTransactions.length;
    const hasTransactions   = (( stagedCount + pendingCount ) > 0 );

    return (
        <React.Fragment>

            <div key = { counter }>
                <TransactionQueueModalBody
                    appState = { appState }
                    open = { open }
                    onClose = { onClose }
                />
            </div>

            <UI.Label
                color = { stagedCount > 0 ? 'green' : 'grey' }
                disabled = { !hasTransactions }
                onClick = {() => { hasTransactions && setOpen ( true )}}
            >
                <UI.Icon name = 'cloud upload'/>
                { pendingCount ? `${ stagedCount }/${ pendingCount }` : `${ stagedCount }` }
            </UI.Label>

        </React.Fragment>
    );
});
