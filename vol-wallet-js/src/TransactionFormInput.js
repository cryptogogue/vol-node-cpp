/* eslint-disable no-whitespace-before-property */

import { Transaction, TRANSACTION_TYPE }    from './Transaction';
import { FIELD_CLASS }                      from './TransactionFormFieldControllers';
import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import * as UI                              from 'semantic-ui-react';

//================================================================//
// KeySelector
//================================================================//
export const KeySelector = observer (( props ) => {

    const { field, controller } = props;

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

    const onChange = ( event, data ) => {
        field.setInputString ( data.value )
        controller.validate ();
    };

    return (
        <UI.Form.Dropdown
            fluid
            search
            selection        
            options         = { options }
            defaultValue    = { defaultKeyName }
            onChange        = { onChange }
        />
    );
});

//================================================================//
// TransactionFormInput
//================================================================//
export const TransactionFormInput = observer (( props ) => {

    const { field, controller } = props;

    const errorMsg      = field.error || '';
    const hasError      = ( errorMsg.length > 0 );

    const onChange = ( event ) => {
        field.setInputString ( event.target.value );
        controller.validate ();
    };

    const commonProps = {
        placeholder:    field.friendlyName,
        name:           field.fieldName,
        value:          field.inputString,
        onChange:       onChange,
        error:          hasError ? errorMsg : false,
    }

    switch ( field.constructor ) {

        case FIELD_CLASS.ACCOUNT_KEY:
            return (
                 <KeySelector
                    field       = { field }
                    controller  = { controller }
                />
            );

        case FIELD_CLASS.CRYPTO_KEY:
            return (
                 <UI.Form.TextArea
                    style = {{ fontFamily: 'monospace' }}
                    rows = { field.rows || 8 }
                    { ...commonProps }
                />
            );

        case FIELD_CLASS.INTEGER:
            return (
                 <UI.Form.Input
                    fluid
                    type = 'number'
                    { ...commonProps }
                />
            );

        case FIELD_CLASS.STRING:
            return (
                 <UI.Form.Input
                    fluid
                    type = 'string'
                    { ...commonProps }
                />
            );

        case FIELD_CLASS.TEXT:
            return (
                 <UI.Form.TextArea
                    rows = { field.rows || 8 }
                    { ...commonProps }
                />
            );
    }

    return <div/>;
});
