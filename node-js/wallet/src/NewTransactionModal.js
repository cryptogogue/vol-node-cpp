/* eslint-disable no-whitespace-before-property */

import { Transaction, TRANSACTION_TYPE }        from './Transaction';
import { TransactionForm }                      from './TransactionForm';
import { makeControllerForTransactionType }     from './TransactionFormController';
import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                             from 'mobx-react';
import React, { useState }                      from 'react';
import * as UI                                  from 'semantic-ui-react';

//----------------------------------------------------------------//
export const gTransactionTypes = [
    TRANSACTION_TYPE.SEND_VOL,
    TRANSACTION_TYPE.ACCOUNT_POLICY,
    TRANSACTION_TYPE.BETA_GET_ASSETS,
    TRANSACTION_TYPE.KEY_POLICY,
    TRANSACTION_TYPE.OPEN_ACCOUNT,
    TRANSACTION_TYPE.PUBLISH_SCHEMA,
    TRANSACTION_TYPE.REGISTER_MINER,
    TRANSACTION_TYPE.RENAME_ACCOUNT,
];

//================================================================//
// DropdownMenu
//================================================================//
const DropdownMenu = observer (( props ) => {

    return (
        <UI.Dropdown
            fluid           = { props.fluid }
            item            = { props.item }
            placeholder     = { props.text ? undefined : props.placeholder }
            text            = { props.text }
        >
            <UI.Dropdown.Menu>
                { props.children }
            </UI.Dropdown.Menu>
        </UI.Dropdown>
    );
});

//================================================================//
// NewTransactionModalBody
//================================================================//
const NewTransactionModalBody = observer (( props ) => {

    const { appState, open, onClose, transactionType, setTransactionType } = props;

    let controller = false;
    if ( transactionType ) {
        controller = hooks.useFinalizable (() => makeControllerForTransactionType ( appState, transactionType ));
    }

    // disabled    = { !entitlements.check ( controller.key.entitlements.policy, transactionType )}

    let options = [];
    for ( let typeID in gTransactionTypes ) {
        const transactionType = gTransactionTypes [ typeID ];
        options.push (
            <UI.Dropdown.Item
                key         = { transactionType }
                text        = { transactionType }
                onClick     = {() => { setTransactionType ( transactionType )}}
                disabled    = { !appState.checkTransactionEntitlements ( transactionType )}
            />
        );
    }

    const submitTransaction = () => {
        appState.pushTransaction ( controller.transaction );
        onClose ();
    }

    const submitEnabled = controller && controller.isCompleteAndErrorFree;

    return (
        <UI.Modal
            size = 'small'
            closeIcon
            onClose = {() => { onClose ()}}
            open = { open }
        >
            <UI.Modal.Header>New Transaction</UI.Modal.Header>
            
            <UI.Modal.Content>

                <UI.Menu>
                    <DropdownMenu
                        fluid
                        item
                        placeholder     = { 'Create Transaction' }
                        text            = { transactionType }
                    >
                        { options }
                    </DropdownMenu>
                </UI.Menu>
                
                <If condition = { controller }>
                    <UI.Segment>
                        <TransactionForm controller = { controller }/>
                    </UI.Segment>
                </If>
            </UI.Modal.Content>

            <UI.Modal.Actions>
                <UI.Button
                    positive
                    disabled = { !submitEnabled }
                    onClick = {() => { submitTransaction ()}}
                >
                    Submit Transaction
                </UI.Button>
            </UI.Modal.Actions>
        </UI.Modal>
    );
});

//================================================================//
// NewTransactionModal
//================================================================//
export const NewTransactionModal = observer (( props ) => {

    const { appState, open } = props;
    const [ counter, setCounter ] = useState ( 0 );
    const [ transactionType, setTransactionType ] = useState ( '' );

    const onClose = () => {
        setCounter ( counter + 1 );
        setTransactionType ( '' );
        props.onClose ();
    }

    return (
        <div key = { `${ transactionType }:${ counter }` }>
            <NewTransactionModalBody
                appState                = { appState }
                open                    = { open }
                onClose                 = { onClose }
                transactionType         = { transactionType }
                setTransactionType      = { setTransactionType }
            />
        </div>
    );
});
