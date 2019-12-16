/* eslint-disable no-whitespace-before-property */

import { Transaction, TRANSACTION_TYPE }    from './Transaction';
import { assert, excel, hooks, Service, SingleColumnContainerView, util } from 'fgc';
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
