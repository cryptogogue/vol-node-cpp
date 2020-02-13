/* eslint-disable no-whitespace-before-property */

import { Transaction, TRANSACTION_TYPE }        from './Transaction';
import { TransactionForm }                      from './TransactionForm';
import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                             from 'mobx-react';
import React, { useState }                      from 'react';
import * as UI                                  from 'semantic-ui-react';

//================================================================//
// TransactionModalBody
//================================================================//
const TransactionModalBody = observer (( props ) => {

    const { appState, open, onClose, title, controller } = props;

    const submitTransaction = () => {
        appState.pushTransaction ( controller.transaction );
        onClose ();
    }

    const submitEnabled = appState.hasAccountInfo && controller && controller.isCompleteAndErrorFree;

    return (
        <UI.Modal
            size = 'small'
            closeIcon
            onClose = {() => { onClose ()}}
            open = { open }
        >
            <UI.Modal.Header>{ title }</UI.Modal.Header>
            
            <UI.Modal.Content>
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
// TransactionModal
//================================================================//
export const TransactionModal = observer (( props ) => {

    const { appState, title, controller } = props;
    const [ counter, setCounter ] = useState ( 0 );

    const open = Boolean ( props.open || controller );

    const onClose = () => {
        setCounter ( counter + 1 );
        props.onClose ();
    }

    return (
        <div key = { `${ counter }` }>
            <TransactionModalBody
                appState                = { appState }
                open                    = { open }
                onClose                 = { onClose }
                title                   = { title }
                controller              = { controller }
            />
        </div>
    );
});