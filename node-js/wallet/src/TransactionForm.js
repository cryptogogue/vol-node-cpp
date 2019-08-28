/* eslint-disable no-whitespace-before-property */

import { Service, useService }              from './Service';
import { Transaction, TRANSACTION_TYPE }    from './Transaction';
import { makeControllerForTransactionType } from './TransactionFormController';
import { TransactionFormInput }             from './TransactionFormInput';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Button, Divider, Dropdown, Form, Icon, Modal, Segment, Select } from 'semantic-ui-react';

//================================================================//
// GenericTransactionForm
//================================================================//
export const TransactionForm = observer (( props ) => {

    const { appState, transactionType, onSubmit } = props;

    const controller = useService (() => makeControllerForTransactionType ( appState, transactionType ));

    // add the fields in order
    let fields = [];
    for ( let field of controller.fields ) {

        const name = field.name;
        const fieldValues = controller.fieldValues;
        const fieldErrors = controller.fieldErrors;

        const value = fieldValues [ name ] !== null ? fieldValues [ name ] : '';
        const error = fieldErrors [ name ];

        const onChange = ( event ) => { controller.handleChange ( event.target.name, event.target.type, event.target.value )};

        fields.push (<TransactionFormInput
            key         = { name }
            appState    = { appState }
            field       = { field }
            value       = { value }
            onChange    = { onChange }
            error       = { error }
        />);
    }

    const isSubmitEnabled = controller.isCompleteAndErrorFree;

    const onClick = () => {
        onSubmit ( controller.transaction );
    }

    return (
        <Form size = "large">
            { fields }
            <Button type = 'button' color = "teal" fluid disabled = { !isSubmitEnabled } onClick = { onClick }>
                Stage Transaction
            </Button>
        </Form>
    );
});
