/* eslint-disable no-whitespace-before-property */

import { Transaction, TRANSACTION_TYPE }    from './Transaction';
import { makeControllerForTransactionType } from './TransactionFormController';
import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import * as UI                              from 'semantic-ui-react';

//================================================================//
// TransactionFormInput
//================================================================//
const TransactionFormInput = observer (( props ) => {

    const { appState, field, value, onChange, error } = props;

    const name = field.name;

    const errorMsg = error || '';
    const hasError = ( errorMsg.length > 0 );

    const commonProps = {
        placeholder:    field.friendlyName,
        name:           name,
        value:          value,
        onChange:       onChange,
        error:          hasError ? errorMsg : false,
    }

    switch ( field.fieldType ) {

        case 'INTEGER':
            return (
                 <UI.Form.Input
                    fluid
                    type = 'number'
                    { ...commonProps }
                />
            );

        case 'STRING':
            return (
                 <UI.Form.Input
                    fluid
                    type = 'string'
                    { ...commonProps }
                />
            );

        case 'TEXT':
            return (
                 <UI.Form.TextArea
                    rows = { field.rows || 8 }
                    { ...commonProps }
                />
            );
    }

    return <div/>;
});

//================================================================//
// KeySelector
//================================================================//
export const KeySelector = observer (( props ) => {

    const { controller } = props;
    const appState = controller.appState;

    const account = appState.account;
    const accountKeyNames = appState.getKeyNamesForTransaction ( controller.type );

    let defaultKeyName = appState.getDefaultAccountKeyName ();
    defaultKeyName = accountKeyNames.includes ( defaultKeyName ) ? defaultKeyName : accountKeyNames [ 0 ];

    const options = [];
    for ( let keyName of accountKeyNames ) {

        const key = account.keys [ keyName ];

        options.push ({
            key:        keyName,
            text:       keyName,
            value:      keyName,
        });
    }

    return (
        <UI.Form.Dropdown
            fluid
            search
            selection        
            options         = { options }
            defaultValue    = { defaultKeyName }
            onChange        = {( event, data ) => { controller.setKeyName ( data.value )}}
        />
    );
});

//================================================================//
// TransactionForm
//================================================================//
export const TransactionForm = observer (( props ) => {

    const { controller } = props;

    // add the fields in order
    let fields = [];
    for ( let field of controller.fields ) {

        const name = field.name;
        const fieldValues = controller.fieldValues;
        const fieldErrors = controller.fieldErrors;

        const value = fieldValues [ name ] !== null ? fieldValues [ name ] : '';
        const error = fieldErrors [ name ];

        const onChange = ( event ) => { controller.handleChange ( field, event.target.type, event.target.value )};

        fields.push (
            <TransactionFormInput
                key         = { name }
                appState    = { controller.appState }
                field       = { field }
                value       = { value }
                onChange    = { onChange }
                error       = { error }
            />
        );
    }

    const balance       = controller.balance;
    const textColor     = balance > 0 ? 'black' : 'red';

    return (
        <UI.Form>
            <UI.Header
                as = 'h4'
                style = {{ color: textColor }}
            >
                Balance: { balance }
            </UI.Header>
            { fields }
            <KeySelector controller = { controller }/>
        </UI.Form>
    );
});
