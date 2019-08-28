/* eslint-disable no-whitespace-before-property */

import { Service, useService }              from './Service';
import { Transaction, TRANSACTION_TYPE }    from './Transaction';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Button, Divider, Dropdown, Form, Icon, Modal, Segment, Select } from 'semantic-ui-react';

//================================================================//
// TransactionFormInput
//================================================================//
export const TransactionFormInput = observer (( props ) => {

    const { appState, field, value, onChange, error } = props;

    const name = field.name;

    const errorMsg = error || '';
    const hasError = ( errorMsg.length > 0 );

    if ( name === 'makerKeyName' ) {

        const accountKeyNames = appState.accountKeyNames;

        const select = [];
        for ( let i in accountKeyNames ) {
            const keyName = accountKeyNames [ i ];
            select.push ({ key: i, text: keyName, value: keyName });
        }

        return (
            <Form.Input
                fluid
                control = { Select }
                options = { select }
                placeholder = 'Account Key'
                name = { name }
                value = { value }
                onChange = { onChange }
            />
        );
    }

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
                 <Form.Input
                    fluid
                    type = 'number'
                    { ...commonProps }
                />
            );

        case 'STRING':
            return (
                 <Form.Input
                    fluid
                    type = 'string'
                    { ...commonProps }
                />
            );

        case 'TEXT':
            return (
                 <Form.TextArea
                    rows = { field.rows || 8 }
                    { ...commonProps }
                />
            );
    }

    return <div/>;
});
